/*
    Copyright 2017 KeycapEmu

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <keycap/root/network/connection.hpp>
#include <keycap/root/network/memory_stream.hpp>
#include <keycap/root/network/service_base.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>

#include <gsl/span>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::redirect_error;
using boost::asio::use_awaitable;

namespace keycap::root::network
{
    connection::connection(boost::asio::ip::tcp::socket socket, service_base& service)
      : connection_base{std::move(socket), service.io_context()}
      , service_{service}
    {
    }

    boost::asio::ip::tcp::socket& connection::socket()
    {
        return socket_;
    }

    void connection::listen()
    {
        co_spawn(
            io_service_,
            [self = utility::shared_from_that(this)] {
                //
                return self->do_read();
            },
            detached);

        co_spawn(
            io_service_,
            [self = utility::shared_from_that(this)] {
                //
                return self->do_write();
            },
            detached);
    }

    void connection::send(memory_stream&& stream)
    {
        send(stream.to_span());
    }

    void connection::send(std::span<uint8_t> data)
    {
        send_packet_queue_.emplace_back(data.begin(), data.end());
        send_timer_.cancel_one();
    }

    void connection::send(std::span<char> data)
    {
        send(std::span(reinterpret_cast<uint8_t*>(data.data()), data.size()));
    }

    data_router& connection::get_router()
    {
        return router_;
    }

    awaitable<void> connection::do_read()
    {
        try
        {
            std::vector<uint8_t> buffer(1024, 0);

            while (socket_.is_open())
            {
                std::size_t n = co_await socket_.async_read_some(boost::asio::buffer(buffer), use_awaitable);
                if (!router_.route_inbound(service_, std::span(buffer.data(), n)))
                {
                    router_.route_updated_link_status(service_, link_status::Down);
                    stop();
                    break;
                }
            }
        }
        catch (std::exception&)
        {
            router_.route_updated_link_status(service_, link_status::Down);
            stop();
        }
    }

    awaitable<void> connection::do_write()
    {
        try
        {
            while (socket_.is_open())
            {
                if (send_packet_queue_.empty())
                {
                    boost::system::error_code ec;
                    co_await send_timer_.async_wait(redirect_error(use_awaitable, ec));
                }
                else
                {
                    co_await boost::asio::async_write(
                        socket_, boost::asio::buffer(send_packet_queue_.front()), use_awaitable);
                    send_packet_queue_.pop_front();
                }
            }
        }
        catch (std::exception&)
        {
            stop();
        }
    }

    void connection::stop()
    {
        socket_.close();
    }
}