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

#include <boost/asio.hpp>

#include <deque>
#include <memory>
#include <vector>

namespace keycap::root::network
{
    class connection_base : public std::enable_shared_from_this<connection_base>
    {
      public:
        connection_base(boost::asio::io_service& ioService)
          : io_service_{ioService}
          , socket_{ioService}
          , write_strand_{ioService}
        {
        }

        virtual ~connection_base()
        {
        }

        virtual void send(std::vector<std::uint8_t> const& data) = 0;

      protected:
        virtual void send_data() = 0;
        virtual void send_data_done(boost::system::error_code const& error) = 0;

        boost::asio::io_service& io_service_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::io_service::strand write_strand_;
        boost::asio::streambuf in_packet_;
        std::deque<std::vector<std::uint8_t>> send_packet_queue_;
    };
}