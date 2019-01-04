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

#include "../types.hpp"
#include "../utility/enum.hpp"
#include "memory_stream.hpp"

namespace keycap::root::network
{
    keycap_enum(registered_command, uint16, Update = 0, Request = 1, );

    // A message that allows RPC like messaging
    struct registered_message
    {
        // CRC32 of the sender, command and payload
        uint32 crc = 0;
        // Unique number of the sender. Used to route the answer back to the sender
        uint64 sender = 0;
        // The command that needs to be executed at the other end
        registered_command command;
        // The command's payload
        memory_stream payload;

        memory_stream encode()
        {
            memory_stream encoder;
            encoder.put<uint64>(sizeof(uint32) + sizeof(uint64) + sizeof(registered_command) + payload.size());
            encoder.put(crc);
            encoder.put(sender);
            encoder.put(command);
            encoder.put(payload);
            return encoder;
        }

        static registered_message decode(memory_stream& decoder)
        {
            registered_message packet;
            decoder.get<uint64>();
            packet.crc = decoder.get<uint32>();
            packet.sender = decoder.get<uint64>();
            packet.command = decoder.get<registered_command>();
            packet.payload = decoder.get_remaining();

            decoder.shrink();

            return packet;
        }

        static bool can_decode(memory_stream const& decoder)
        {
            return decoder.peek<uint64>() <= decoder.size();
        }
    };
}