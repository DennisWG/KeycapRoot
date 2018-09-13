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

#pragma once

#include "../utility/utility.hpp"
#include "connection_base.hpp"
#include "data_router.hpp"
#include "service_base.hpp"

namespace keycap::root::network
{
    class service_base;

    class connection : public connection_base
    {
      public:
        connection(service_base& service)
          : connection_base{service.io_service()}
          , service_{service}
        {
        }

        // Returns the socket used by the connection handler
        boost::asio::ip::tcp::socket& socket()
        {
            return socket_;
        }

        // The connection handler will start to asynchronously listen for incoming data
        void listen()
        {
            read_packet();
        }

        // Sends the given data asynchronously
        void send(std::vector<std::uint8_t> const& data) override
        {
            io_service_.post(write_strand_.wrap([ self = utility::shared_from_that(this), data ]() {
                bool isWriteInProgress = !self->send_packet_queue_.empty();
                self->send_packet_queue_.push_back(std::move(data));

                if (!isWriteInProgress)
                    self->send_data();
            }));
        }

        data_router& get_router()
        {
            return router_;
        }

      private:
        void read_packet()
        {
            auto buffer = in_packet_.prepare(512);
            socket_.async_read_some(
                buffer, [self
                         = utility::shared_from_that(this)](boost::system::error_code const& error, size_t bytesRead) {
                    self->in_packet_.commit(bytesRead);
                    self->read_packet_done(error, bytesRead);
                });
        }

        void read_packet_done(boost::system::error_code const& error, size_t numBytesRead)
        {
            if (error)
            {
                router_.route_updated_link_status(service_, link_status::Down);
                return;
            }

            std::vector<uint8_t> buffer;
            buffer.resize(numBytesRead);

            in_packet_.sgetn(reinterpret_cast<char*>(buffer.data()), numBytesRead);

            if (router_.route_inbound(service_, buffer))
                read_packet();
            else
                router_.route_updated_link_status(service_, link_status::Down);
        }

        void send_data() override
        {
            boost::asio::async_write(
                socket_, boost::asio::buffer(send_packet_queue_.front()),
                [self = utility::shared_from_that(this)](boost::system::error_code const& error, size_t) {
                    self->send_data_done(error);
                });
        }

        void send_data_done(boost::system::error_code const& error) override
        {
            if (error)
                return;

            send_packet_queue_.pop_front();
            if (!send_packet_queue_.empty())
                send_data();
        }

      protected:
        data_router router_;
        service_base& service_;
    };
}