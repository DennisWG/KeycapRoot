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

#include <keycap/root/utility/random.hpp>

#include <random>

namespace keycap::root::utility
{
    static thread_local std::random_device random_device;
    static thread_local std::mt19937_64 generator{random_device()};

    uint8_t random_ui8(uint8_t const min, uint8_t const max)
    {
        std::uniform_int_distribution<uint16_t> dist(min, max);
        return static_cast<uint8_t>(dist(generator));
    }

    uint16_t random_ui16(uint16_t const min, uint16_t const max)
    {
        std::uniform_int_distribution<uint16_t> dist(min, max);
        return dist(generator);
    }

    uint32_t random_ui32(uint32_t const min, uint32_t const max)
    {
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return dist(generator);
    }

    uint64_t random_ui64(uint64_t const min, uint64_t const max)
    {
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return dist(generator);
    }

    int8_t random_i8(int8_t const min, int8_t const max)
    {
        std::uniform_int_distribution<int16_t> dist(min, max);
        return static_cast<int8_t>(dist(generator));
    }

    int16_t random_i16(int16_t const min, int16_t const max)
    {
        std::uniform_int_distribution<int16_t> dist(min, max);
        return dist(generator);
    }

    int32_t random_i32(int32_t const min, int32_t const max)
    {
        std::uniform_int_distribution<int32_t> dist(min, max);
        return dist(generator);
    }

    int64_t random_i64(int64_t const min, int64_t const max)
    {
        std::uniform_int_distribution<int64_t> dist(min, max);
        return dist(generator);
    }

    float random_float(float const min, float const max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator);
    }

    double random_double(double const min, double const max)
    {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator);
    }

    char random_char(char const min, char const max)
    {
        return static_cast<char>(random_i8(static_cast<int8_t>(min), static_cast<int8_t>(max)));
    }

    std::string random_string(size_t len, std::string const& allowed_chars)
    {
        std::uniform_int_distribution<size_t> dist{0, allowed_chars.length() - 1};

        std::string ret;
        std::generate_n(std::back_inserter(ret), len, [&] { return allowed_chars[dist(generator)]; });

        return ret;
    }
}