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

#include <keycap/root/utility/md5.hpp>

#include <rapidcheck/catch.h>

namespace util = keycap::root::utility;

TEST_CASE("md5")
{
    SECTION("Hashing single string must yield a valid result")
    {
        std::array<uint8, 16> expected_result{0xb1, 0x0a, 0x8d, 0xb1, 0x64, 0xe0, 0x75, 0x41,
                                              0x05, 0xb7, 0xa9, 0x9b, 0xe7, 0x2e, 0x3f, 0xe5};

        REQUIRE(util::md5("Hello World") == expected_result);
    }

    SECTION("Hashing multiple values must yield a valid result")
    {
        std::array<uint8, 16> expected_result{0xb1, 0x0a, 0x8d, 0xb1, 0x64, 0xe0, 0x75, 0x41,
                                              0x05, 0xb7, 0xa9, 0x9b, 0xe7, 0x2e, 0x3f, 0xe5};

        REQUIRE(util::md5("Hello", ' ', "World") == expected_result);
    }

    SECTION("Subsequent hashes must yield the same results", "[regression]")
    {
        std::array<uint8, 16> expected_result{0xb1, 0x0a, 0x8d, 0xb1, 0x64, 0xe0, 0x75, 0x41,
                                              0x05, 0xb7, 0xa9, 0x9b, 0xe7, 0x2e, 0x3f, 0xe5};

        REQUIRE(util::md5("Hello", ' ', "World") == expected_result);
        REQUIRE(util::md5("Hello", ' ', "World") == expected_result);
        REQUIRE(util::md5("Hello", ' ', "World") == expected_result);
    }
}