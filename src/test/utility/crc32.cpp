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

#include <keycap/root/utility/crc32.hpp>

#include <rapidcheck/catch.h>

namespace util = keycap::root::utility;

TEST_CASE("crc32")
{
    SECTION("Hashing single string must yield a valid result")
    {
        REQUIRE(0x4A17B156 == util::crc32(std::string("Hello World")));
    }

    SECTION("Hashing multiple values must yield a valid result")
    {
        REQUIRE(0x4A17B156 == util::crc32(std::string("Hello"), ' ', std::string("World")));
    }

    SECTION("Subsequent hashes must yield the same results", "[regression]")
    {
        REQUIRE(0x4A17B156 == util::crc32(std::string("Hello"), ' ', std::string("World")));
        REQUIRE(0x4A17B156 == util::crc32(std::string("Hello"), ' ', std::string("World")));
        REQUIRE(0x4A17B156 == util::crc32(std::string("Hello"), ' ', std::string("World")));
    }
}