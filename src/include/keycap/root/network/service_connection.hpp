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

#include "../utility/crc32.hpp"
#include "connection.hpp"
#include "registered_message.hpp"

namespace keycap::root::network
{
    class service_connection : public connection, public message_handler
    {
        friend class data_router;

      public:
        service_connection(service_base& base_service)
          : connection{base_service}
        {
        }

        virtual bool on_data(data_router const& router,  service_type service, uint64 sender, memory_stream& stream) = 0;

        void send_answer(uint64 receiver, memory_stream const& payload)
        {
            registered_message msg;
            msg.crc = utility::crc32(receiver, registered_command::Request, payload);
            msg.sender = receiver;
            msg.command = registered_command::Request;
            msg.payload = payload;

            auto stream = msg.encode();
            send(std::vector<uint8>{stream.data(), stream.data() + stream.size()});
        }

      private:
        bool on_data(data_router const& router, service_type service, std::vector<uint8_t> const& data) final
        {
            input_stream_.put(gsl::make_span(data));

            if(!registered_message::can_decode(input_stream_))
                return true;

            auto msg = registered_message::decode(input_stream_);

            if (!utility::validate_crc32(msg.crc, msg.sender, msg.command, msg.payload))
            {
                // TODO: implement error handling callback
                return false;
            }

            return on_data(router, service, msg.sender, msg.payload);
        }

        memory_stream input_stream_;
    };
}