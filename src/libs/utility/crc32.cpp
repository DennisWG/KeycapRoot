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

#include <keycap/root/network/memory_stream.hpp>
#include <keycap/root/utility/crc32.hpp>

namespace keycap::root::utility
{
    namespace impl
    {
        template <>
        void hash<network::memory_stream>(boost::crc_32_type& crc, network::memory_stream const& stream)
        {
            crc.process_bytes(stream.data(), stream.size());
        }

        template <>
        void hash<std::string>(boost::crc_32_type& crc, std::string const& str)
        {
            crc.process_bytes(str.data(), str.size());
        }
    }
}