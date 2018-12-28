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

#include <array>

namespace keycap::root::compression::zip
{
    namespace impl
    {
        std::vector<uint8_t> compress(uint8_t* begin, unsigned long size)
        {
            std::vector<uint8_t> buffer;
            buffer.resize(compressBound(size));

            unsigned long buffer_size = static_cast<unsigned long>(buffer.size());

            if (::compress2(buffer.data(), &buffer_size, begin, size, 8) != Z_OK)
                return {};

            buffer.resize(buffer_size);

            return buffer;
        }

        void inflate(std::vector<uint8_t>& data, uint8_t* ptr, unsigned long size)
        {
            constexpr int buffer_size = 1024;
            std::array<uint8_t, buffer_size> in{};
            std::array<uint8_t, buffer_size> out{};
            int ret = Z_OK;

            z_stream stream{};
            if (inflateInit(&stream) != Z_OK)
                return;

            do
            {
                if (size - stream.total_in < buffer_size)
                    stream.avail_in = size - stream.total_in;
                else
                    stream.avail_in = buffer_size;

                stream.next_in = ptr + stream.total_in;

                do
                {
                    stream.avail_out = buffer_size;
                    stream.next_out = out.data();

                    ret = inflate(&stream, Z_NO_FLUSH);

                    data.insert(data.end(), out.begin(), out.end());

                    switch (ret)
                    {
                        case Z_STREAM_ERROR:
                            [[fallthrough]];
                        case Z_NEED_DICT:
                            [[fallthrough]];
                        case Z_DATA_ERROR:
                            [[fallthrough]];
                        case Z_MEM_ERROR:
                            inflateEnd(&stream);
                    }
                } while (stream.avail_out == 0);

            } while (ret != Z_STREAM_END);

            data.resize(stream.total_out);

            inflateEnd(&stream);
        }

        std::vector<uint8_t> decompress(uint8_t* begin, unsigned long size)
        {
            std::vector<uint8_t> buffer;

            inflate(buffer, begin, size);

            return buffer;
        }
    }
}