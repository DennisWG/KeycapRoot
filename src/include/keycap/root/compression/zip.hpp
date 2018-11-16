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

#pragma once

#include <cassert>
#include <cstdint>
#include <vector>

namespace keycap::root::compression::zip
{
    namespace impl
    {
        std::vector<uint8_t> compress(uint8_t* begin, unsigned long size);
        std::vector<uint8_t> decompress(uint8_t* begin, unsigned long size);
    }

    template <typename Iter>
    std::vector<uint8_t> compress(Iter begin, Iter end)
    {
        constexpr unsigned long minus_one = -1;
        assert((end - begin) < minus_one, "The range mustn't be bigger than 2^32 bytes!");

        if (begin == end)
            return {};

        return impl::compress(reinterpret_cast<uint8_t*>(&*begin), static_cast<unsigned long>(end - begin));
    }

    template <typename Iter>
    std::vector<uint8_t> decompress(Iter begin, Iter end)
    {
        constexpr unsigned long minus_one = -1;
        assert((end - begin) < minus_one, "The range mustn't be bigger than 2^32 bytes!");

        if (begin == end)
            return {};

        return impl::decompress(reinterpret_cast<uint8_t*>(&*begin), static_cast<unsigned long>(end - begin));
    }
}