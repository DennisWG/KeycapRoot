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

#include <keycap/root/compression/zip.hpp>
#include <keycap/root/network/memory_stream.hpp>

namespace keycap::root::network
{
    void memory_stream::decompress(uint32_t length)
    {
        auto buffer = compression::zip::impl::decompress(buffer_.data() + read_position_, length);

        auto new_size = read_position_ + buffer.size();
        auto current_size = buffer_.size();

        if (new_size > current_size)
            buffer_.resize(new_size);

        std::copy(buffer.begin(), buffer.end(), buffer_.begin() + read_position_);
    }

    bool memory_stream::has_data_remaining() const
    {
        return size() == 0;
    }
}