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

namespace keycap::root::network
{
    class service_base;
    class memory_stream;

    class connection : public connection_base
    {
      public:
        connection(service_base& service);

        // Returns the socket used by the connection handler
        boost::asio::ip::tcp::socket& socket();

        // The connection handler will start to asynchronously listen for incoming data
        void listen();

        void send(memory_stream const& stream);

        // Sends the given data asynchronously
        void send(std::vector<std::uint8_t> const& data) override;

        data_router& get_router();

      private:
        void read_packet();

        void read_packet_done(boost::system::error_code const& error, size_t numBytesRead);

        void send_data() override;

        void send_data_done(boost::system::error_code const& error) override;

      protected:
        data_router router_;
        service_base& service_;
    };
}