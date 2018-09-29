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

#include "../types.hpp"

#include <array>
#include <vector>

namespace keycap::root::cryptography
{
    class ARC4
    {
      public:
        explicit ARC4(std::vector<uint8> const& key);

        void process(uint8* data, size_t size)
        {
            process_(data, size);
        }

      private:
        void process_(uint8* begin, size_t size);

        constexpr static size_t block_size = 256;
        std::array<uint8, block_size> block_;

        uint8 x = 0;
        uint8 y = 0;
    };
}