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

#include <boost/crc.hpp>

namespace keycap::root::network
{
    class memory_stream;
}

namespace keycap::root::utility
{
    namespace impl
    {
        template <typename T>
        void hash(boost::crc_32_type& crc, T const& value)
        {
            crc.process_bytes(&value, sizeof(T));
        }

        template <>
        void hash<network::memory_stream>(boost::crc_32_type& crc, network::memory_stream const& stream);

        template <>
        void hash<std::string>(boost::crc_32_type& crc, std::string const& str);

        template <>
        void hash<std::array<uint8, 256>>(boost::crc_32_type& crc, std::array<uint8, 256> const& ar);
    }

    template <typename... ARGS>
    uint32 crc32(ARGS&&... args)
    {
        thread_local boost::crc_32_type crc;
        crc.reset();

        (impl::hash(crc, args), ...);

        return crc.checksum();
    }

    template <typename... ARGS>
    bool validate_crc32(uint32 crc, ARGS&&... args)
    {
        return crc == crc32(args...);
    }
}