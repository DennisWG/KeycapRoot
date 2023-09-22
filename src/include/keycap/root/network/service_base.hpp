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

#include "service_type.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace keycap::root::network
{
    class service_base
    {
      public:
        service_base(service_type type);

        service_type type();

        boost::asio::io_context& io_context();

        // Returns wether or not the service has been started.
        bool running() const
        {
            return running_;
        }

<<<<<<< HEAD
        virtual void handle_new_connection(boost::asio::ip::tcp::socket socket) = 0;
=======
        virtual ~service_base() = default;

        service_type type()
        {
            return type_;
        }
>>>>>>> fa46a7e (Make the project compile again on modern systems.)

      protected:
        boost::asio::ip::tcp::endpoint resolve(std::string const& host, uint16_t port);

        void listen();

        void connect();

        boost::asio::io_context io_context_;
        boost::asio::ip::tcp::endpoint endpoint_;

        bool running_ = false;

      private:
        service_type type_;
    };
}