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

#include <keycap/root/cryptography/ARC4.hpp>

#include <algorithm>
#include <numeric>

namespace keycap::root::cryptography
{
    ARC4::ARC4(std::vector<uint8> const& key)
    {
        uint8 j = 0;
        uint8 k = 0;
        
        std::iota(std::begin(block_), std::end(block_), 0);

        for (int i = 0; i < block_size; ++i)
        {
            k = key[j] + block_[i] + k;
            std::swap(block_[i], block_[k]);
            j = (j + 1) % key.size();
        }
    }

    void ARC4::process_(uint8* begin, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            x = x + 1;
            y = block_[x] + y;
            std::swap(block_[x], block_[y]);

            uint8 j = block_[x] + block_[y];
            *(begin + i) = *(begin + i) ^ block_[j];
        }
    }
}