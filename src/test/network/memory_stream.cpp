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

#include <keycap/root/network/memory_stream.hpp>

#include <rapidcheck/catch.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace net = keycap::root::network;

TEST_CASE("memory_stream")
{
    net::memory_stream stream;

    SECTION("An empty stream can't be read")
    {
        REQUIRE_THROWS(stream.get<int>());
    }

    SECTION("The size of an empty stream is always 0")
    {
        size_t const expectedSize = 0;

        REQUIRE(stream.size() == expectedSize);
    }

    SECTION("Calling memory_stream::shrink() on empty stream should do nothing")
    {
        size_t const expectedSize = 0;

        REQUIRE_NOTHROW(stream.shrink());
        REQUIRE(stream.size() == expectedSize);
    }

    SECTION("Streams operate using the FIFO principle")
    {
        uint32_t const first = 0xDEADBEEF;
        double const second = M_PI;

        stream.put(first);
        stream.put(second);

        REQUIRE(stream.get<uint32_t>() == first);
        REQUIRE(stream.get<double>() == second);
    }

    SECTION("Putting vectors on the stream")
    {
        size_t const expectedSize = 8;
        uint32_t const first = 0xB00B5;
        uint32_t const second = 0xC0CAC01A;

        std::vector<uint32_t> data{first, second};

        stream.put(gsl::make_span(data));

        REQUIRE(stream.size() == expectedSize);
        REQUIRE(stream.get<uint32_t>() == first);
        REQUIRE(stream.get<uint32_t>() == second);
    }

    SECTION("memory_stream::shrink() must only remove data that has already been read")
    {
        size_t const expectedSize = 2;
        uint8_t const first = 3;
        uint8_t const second = 2;

        std::array<uint8_t, 4> data{8, 4, first, second};
        stream.put(gsl::make_span(data));

        stream.get<uint8_t>();
        stream.get<uint8_t>();

        stream.shrink();

        REQUIRE(stream.size() == expectedSize);
        REQUIRE(stream.get<uint8_t>() == first);
        REQUIRE(stream.get<uint8_t>() == second);
    }

    SECTION("memory_stream::Peek() must not modify the stream in any way")
    {
        size_t const expectedSize = 5;
        uint32_t const first = 1337;
        uint8_t const second = 255;

        stream.put(first);
        stream.put(second);

        REQUIRE(stream.peek<uint8_t>(4) == second);

        REQUIRE(stream.size() == expectedSize);
        REQUIRE(stream.get<uint32_t>() == first);
        REQUIRE(stream.get<uint8_t>() == second);
    }

    SECTION("memory_stream::GetString() must return a valid std::string")
    {
        std::string const str = "Foobar";
        uint32_t const junk = 0xDEADBEEF;

        stream.put(str);
        stream.put(junk);

        REQUIRE(stream.get_string(str.size()) == str);
        REQUIRE(stream.get<uint32_t>() == junk);
    }

    SECTION("Putting arrays on and retreiving them again from the stream")
    {
        using ElementType = int;
        size_t const NumElements = 4;
        using ArrayType = std::array<ElementType, NumElements>;

        ArrayType arr{1, 2, 3, 4};
        uint32_t const junk = 0xDEADBEEF;

        stream.put(arr);
        stream.put(junk);

        auto out = stream.get<ElementType, NumElements>();

        for (int i = 0; i < NumElements; ++i)
            REQUIRE(arr[i] == out[i]);

        REQUIRE(stream.get<uint32_t>() == junk);
    }

    SECTION("memory_stream::has_data_remaining must return false on empty stream", "[regression]")
    {
        REQUIRE(stream.has_data_remaining() == false);
    }

    SECTION("memory_stream::has_data_remaining must return true on non-empty stream", "[regression]")
    {
        stream.put(1);
        REQUIRE(stream.has_data_remaining() != false);
    }

    SECTION("memory_stream::has_data_remaining must return true on non-empty stream", "[regression]")
    {
        stream.put(1);
        stream.put(2);
        stream.get<int>();

        REQUIRE(stream.has_data_remaining() != false);
    }
}
