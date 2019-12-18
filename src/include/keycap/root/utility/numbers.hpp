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

#include <array>

namespace keycap::root::utility
{
    namespace Impl
    {
        template <typename PTR, typename T>
        void to_array_impl(PTR data, T value)
        {
            std::memcpy(data, &value, sizeof(value));
        }
    }

    // Converts an int16 an array
    std::array<byte, 2> to_array(int16 number)
    {
        std::array<byte, 2> value;
        Impl::to_array_impl(value.data(), number);
        return value;
    }

    // Converts an int32 an array
    std::array<byte, 4> to_array(int32 number)
    {
        std::array<byte, 4> value;
        Impl::to_array_impl(value.data(), number);
        return value;
    }

    // Converts an int64 an array
    std::array<byte, 8> to_array(int64 number)
    {
        std::array<byte, 8> value;
        Impl::to_array_impl(value.data(), number);
        return value;
    }

    // Converts an uint16 an array
    std::array<byte, 2> to_array(uint16 number)
    {
        std::array<byte, 2> value;
        Impl::to_array_impl(value.data(), number);
        return value;
    }

    // Converts an uint32 an array
    std::array<byte, 4> to_array(uint32 number)
    {
        std::array<byte, 4> value;
        Impl::to_array_impl(value.data(), number);
        return value;
    }

    // Converts an uint64 an array
    std::array<byte, 8> to_array(uint64 number)
    {
        std::array<byte, 8> value;
        Impl::to_array_impl(value.data(), number);
        return value;
    }

    // Converts an array to the given value
    // Hint:
    // If the number of bytes within the array does not equal to sizeof(RESULT_TYPE) the smaller of the two will
    // be chosen to avoid over-flowing
    template <typename T, size_t SIZE, typename RESULT_TYPE>
    RESULT_TYPE from_array(std::array<T, SIZE> const& array)
    {
        static_assert(SIZE == sizeof(RESULT_TYPE), "Size mismatch!");
        RESULT_TYPE value = 0;
        std::memcpy(&value, array.data(), std::min(array.size(), sizeof(RESULT_TYPE)));
        return value;
    }

    // Converts a vector to the given value
    // Hint:
    // If the number of bytes within the vector does not equal to sizeof(RESULT_TYPE) the smaller of the two will
    // be chosen to avoid over-flowing
    template <typename T, typename RESULT_TYPE>
    RESULT_TYPE from_vector(std::vector<T> const& vector)
    {
        RESULT_TYPE value = 0;
        std::memcpy(&value, vector.data(), std::min(vector.size(), sizeof(RESULT_TYPE)));
        return value;
    }
}