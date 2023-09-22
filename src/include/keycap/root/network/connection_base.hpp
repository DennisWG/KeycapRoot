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

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include <deque>
#include <memory>
#include <span>
#include <vector>

namespace keycap::root::network
{
    class connection_base : public std::enable_shared_from_this<connection_base>
    {
      public:
        connection_base(boost::asio::ip::tcp::socket socket, boost::asio::io_context& ioService)
          : io_service_{ioService}
          , socket_{std::move(socket)}
          , write_strand_{ioService}
          , send_timer_{ioService}
        {
            send_timer_.expires_at(std::chrono::steady_clock::time_point::max());
        }

        virtual ~connection_base()
        {
        }

        virtual void send(std::span<uint8_t> data) = 0;

      protected:
        boost::asio::io_context& io_service_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::io_context::strand write_strand_;
        boost::asio::streambuf in_packet_;
        std::deque<std::vector<std::uint8_t>> send_packet_queue_;

        boost::asio::steady_timer send_timer_;
    };
}