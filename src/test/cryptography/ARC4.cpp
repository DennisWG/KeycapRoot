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

#include <rapidcheck/catch.h>

namespace crypt = keycap::root::cryptography;

TEST_CASE("ARC4")
{
    SECTION("Processing must yield a valid result")
    {
        crypt::ARC4 arc4{std::vector<uint8>{0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE,
                                            0x2F, 0x43, 0x67, 0xCE}};

        std::vector<uint8> data{0, 1, 2, 3, 4};

        arc4.process(data.data(), data.size());

        REQUIRE(data[0] == 0x60);
        REQUIRE(data[1] == 0x23);
        REQUIRE(data[2] == 0x61);
        REQUIRE(data[3] == 0x05);
        REQUIRE(data[4] == 0x2b);
    }

    SECTION("Processing multiple times must yield a valid result")
    {
        crypt::ARC4 arc4{std::vector<uint8>{0xC2, 0xB3, 0x72, 0x3C, 0xC6, 0xAE, 0xD9, 0xB5, 0x34, 0x3C, 0x53, 0xEE,
                                            0x2F, 0x43, 0x67, 0xCE}};

        std::vector<uint8> data{0, 1, 2, 3, 4};
        std::vector<uint8> data2{4, 3, 2, 1, 0};

        arc4.process(data.data(), data.size());
        arc4.process(data2.data(), data.size());

        REQUIRE(data2[0] == 0x96);
        REQUIRE(data2[1] == 0x5d);
        REQUIRE(data2[2] == 0xf4);
        REQUIRE(data2[3] == 0x7c);
        REQUIRE(data2[4] == 0x64);
    }
}