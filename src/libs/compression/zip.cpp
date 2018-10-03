/*
    Copyright 2018 KeycapEmu

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

#include <keycap/root/compression/zip.hpp>

#include <zlib.h>

namespace keycap::root::compression::zip
{
    namespace impl
    {
        std::vector<uint8_t> compress(uint8_t* begin, unsigned long size)
        {
            std::vector<uint8_t> buffer;
            buffer.resize(compressBound(size));

            unsigned long buffer_size = static_cast<unsigned long>(buffer.size());

            if (::compress(buffer.data(), &buffer_size, begin, size) != Z_OK)
                return {};

            buffer.resize(buffer_size);

            return buffer;
        }

        std::vector<uint8_t> decompress(uint8_t* begin, unsigned long size)
        {
            std::vector<uint8_t> buffer;
            buffer.resize(size);

            if (::uncompress(buffer.data(), &size, begin, size) != Z_OK)
                return {};

            buffer.resize(size);

            return buffer;
        }
    }
}