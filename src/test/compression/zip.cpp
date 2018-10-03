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

#include <keycap/root/compression/zip.hpp>

#include <rapidcheck/catch.h>

#include <zlib.h>

using namespace keycap::root::compression;

TEST_CASE("Zip")
{
    SECTION("Compressing constant data must always yield the same result")
    {
        std::string input = "Hello, World!";
        std::vector<uint8_t> expected
            = {120, 156, 243, 72, 205, 201, 201, 215, 81, 8, 207, 47, 202, 73, 81, 4, 0, 31, 158, 4, 106};

        auto output = zip::compress(input.begin(), input.end());

        REQUIRE(std::equal(expected.begin(), expected.end(), output.begin()));
    }

    SECTION("Compressing constant data must always yield the same result")
    {
        std::vector<uint8_t> input
            = {120, 156, 243, 72, 205, 201, 201, 215, 81, 8, 207, 47, 202, 73, 81, 4, 0, 31, 158, 4, 106};
        std::string expected = "Hello, World!";

        auto output = zip::decompress(input.begin(), input.end());

        REQUIRE(std::equal(expected.begin(), expected.end(), output.begin()));
    }

    rc::prop("Decompressing compressed arbitrary data must always be the same as the input", [](std::string input) {

        auto output = zip::compress(input.begin(), input.end());
        auto decompressed = zip::decompress(output.begin(), output.end());

        std::string str{decompressed.begin(), decompressed.end()};

        REQUIRE(str == input);
    });
}