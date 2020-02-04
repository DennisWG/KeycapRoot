/*
    Copyright 2020 KeycapEmu

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

#include <keycap/root/network/service_base.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

namespace keycap::root::network
{
    service_base::service_base(service_type type)
      : type_{type}
    {
    }

    service_type service_base::type()
    {
        return type_;
    }

    boost::asio::io_context& service_base::io_context()
    {
        return io_context_;
    }

    boost::asio::ip::tcp::endpoint service_base::resolve(std::string const& host, uint16_t port)
    {
        boost::asio::ip::tcp::resolver resolver{io_context_};
        auto endpoint = resolver.resolve({host, ""})->endpoint();
        endpoint.port(port);

        return endpoint;
    }

    awaitable<void> do_listen(service_base* self, tcp::acceptor acceptor)
    {
        acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address{true});

        while (self->running())
        {
            self->handle_new_connection(co_await acceptor.async_accept(use_awaitable));
        }
    }

    void service_base::listen()
    {
        running_ = true;

        co_spawn(
            io_context_,
            [this] {
                //
                return do_listen(this, tcp::acceptor(io_context_, endpoint_));
            },
            detached);
    }

    void service_base::connect()
    {
        auto handler = [](service_base* self, auto endpoint) -> awaitable<void> {
            try
            {
                tcp::socket socket(self->io_context());
                co_await socket.async_connect(endpoint, use_awaitable);
                self->handle_new_connection(std::move(socket));
            }
            catch (std::exception& ex)
            {
                std::printf("error: %s\n", ex.what());
            }
        };

        co_spawn(
            io_context_,
            [this, &handler] {
                //
                return handler(this, endpoint_);
            },
            detached);
    }
}