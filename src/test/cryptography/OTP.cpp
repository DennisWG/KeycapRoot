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

#include <keycap/root/cryptography/OTP.hpp>

#include <rapidcheck/catch.h>

namespace crypt = keycap::root::cryptography;

TEST_CASE("OTP")
{
    SECTION("HOTP must generate correct consecutive one-time passwords")
    {
        std::string const key = "GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ";
        int constexpr num_tests = 10;
        int constexpr num_digits = 6;
        std::array<char[num_digits + 1], num_tests> constexpr results = {
            // clang-format off
            "755224",
            "287082",
            "359152",
            "969429",
            "338314",
            "254676",
            "287922",
            "162583",
            "399871",
            "520489",
            // clang-format on
        };

        for (int i = 0; i < num_tests; ++i)
        {
            REQUIRE(results[i] == crypt::HOTP::generate(key, i, num_digits));
        }
    }

    SECTION("TOTP must generate correct consecutive one-time passwords")
    {
        std::string const key = "GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ";
        uint64 constexpr start = 0;
        uint64 constexpr step = 30;

        int constexpr num_tests = 6;
        int constexpr num_digits = 8;

        uint64 constexpr test_time[num_tests] = {59, 1111111109, 1111111111, 1234567890, 2000000000, 20000000000};
        std::array<char[num_digits + 1], num_tests> constexpr results = {
            // clang-format off
            "94287082",
            "07081804",
            "14050471",
            "89005924",
            "69279037",
            "65353130",
            // clang-format on
        };

        for (int i = 0; i < num_tests; ++i)
        {
            REQUIRE(results[i] == crypt::TOTP::generate(key, test_time[i], start, step, num_digits));
        }
    }
}