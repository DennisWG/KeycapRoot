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

#include <array>
#include <cstdint>
#include <limits>
#include <string>

namespace keycap::root::utility
{
    // Returns a random uint8 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    uint8_t random_ui8(
        uint8_t const min = std::numeric_limits<uint8_t>::min(),
        uint8_t const max = std::numeric_limits<uint8_t>::max());

    // Returns a random uint16 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    uint16_t random_ui16(
        uint16_t const min = std::numeric_limits<uint16_t>::min(),
        uint16_t const max = std::numeric_limits<uint16_t>::max());

    // Returns a random uint32 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    uint32_t random_ui32(
        uint32_t const min = std::numeric_limits<uint32_t>::min(),
        uint32_t const max = std::numeric_limits<uint32_t>::max());

    // Returns a random uint64 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    uint64_t random_ui64(
        uint64_t const min = std::numeric_limits<uint64_t>::min(),
        uint64_t const max = std::numeric_limits<uint64_t>::max());

    // Returns a random int8 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    int8_t random_i8(
        int8_t const min = std::numeric_limits<int8_t>::min(), int8_t const max = std::numeric_limits<int8_t>::max());

    // Returns a random int16 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    int16_t random_i16(
        int16_t const min = std::numeric_limits<int16_t>::min(),
        int16_t const max = std::numeric_limits<int16_t>::max());

    // Returns a random int32 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    int32_t random_i32(
        int32_t const min = std::numeric_limits<int32_t>::min(),
        int32_t const max = std::numeric_limits<int32_t>::max());

    // Returns a random int64 between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    int64_t random_i64(
        int64_t const min = std::numeric_limits<int64_t>::min(),
        int64_t const max = std::numeric_limits<int64_t>::max());

    // Returns a random float between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    float random_float(
        float const min = std::numeric_limits<float>::min(), float const max = std::numeric_limits<float>::max());

    // Returns a random double between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    double random_double(
        double const min = std::numeric_limits<double>::min(), double const max = std::numeric_limits<double>::max());

    // Returns a random char between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    char
    random_char(char const min = std::numeric_limits<char>::min(), char const max = std::numeric_limits<char>::max());

    // Returns a random array with values between min and max (inclusive)
    // Very fast, not cryptographically safe
    // Thread safe.
    template <typename T, size_t N>
    std::array<T, N>
    random_array(T const min = std::numeric_limits<T>::min(), T const max = std::numeric_limits<T>::max())
    {
        std::array<T, N> arr;

        for (size_t i = 0; i < N; ++i)
        {
            if constexpr (std::is_same_v<std::remove_cv_t<T>, uint8_t>)
                arr[i] = random_ui8(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, uint16_t>)
                arr[i] = random_ui16(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, uint32_t>)
                arr[i] = random_ui32(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, uint64_t>)
                arr[i] = random_ui64(min, max);

            else if constexpr (std::is_same_v<std::remove_cv_t<T>, int8_t>)
                arr[i] = random_i8(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, int16_t>)
                arr[i] = random_i16(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, int32_t>)
                arr[i] = random_i32(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, int64_t>)
                arr[i] = random_i64(min, max);

            else if constexpr (std::is_same_v<std::remove_cv_t<T>, float>)
                arr[i] = random_float(min, max);
            else if constexpr (std::is_same_v<std::remove_cv_t<T>, double>)
                arr[i] = random_double(min, max);

            else if constexpr (std::is_same_v<std::remove_cv_t<T>, char>)
                arr[i] = random_char(min, max);
        }

        return arr;
    }

    namespace impl
    {
        std::string const default_chars = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    }

    // Returns a random string with values with the given length (inclusive) and the given allowed characters
    // Very fast, not cryptographically safe
    // Thread safe.
    std::string random_string(size_t len = 15, std::string const& allowed_chars = impl::default_chars);
}