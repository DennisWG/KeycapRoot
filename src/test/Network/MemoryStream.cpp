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

#include <Keycap/Root/Network/MemoryStream.hpp>

#include <rapidcheck/catch.h>

#define _USE_MATH_DEFINES
#include <math.h>

namespace net = Keycap::Root::Network;

TEST_CASE("MemoryStream")
{
    net::MemoryStream stream;

    SECTION("An empty stream can't be read")
    {
        REQUIRE_THROWS(stream.Get<int>());
    }

    SECTION("The size of an empty stream is always 0")
    {
        size_t const expectedSize = 0;

        REQUIRE(stream.Size() == expectedSize);
    }

    SECTION("Calling MemoryStream::Shrink() on empty stream should do nothing")
    {
        size_t const expectedSize = 0;

        REQUIRE_NOTHROW(stream.Shrink());
        REQUIRE(stream.Size() == expectedSize);
    }

    SECTION("Streams operate using the FIFO principle")
    {
        uint32_t const first = 0xDEADBEEF;
        double const second = M_PI;

        stream.Put(first);
        stream.Put(second);

        REQUIRE(stream.Get<uint32_t>() == first);
        REQUIRE(stream.Get<double>() == second);
    }

    SECTION("Putting vectors on the stream")
    {
        size_t const expectedSize = 8;
        uint32_t const first = 0xB00B5;
        uint32_t const second = 0xC0CAC01A;

        std::vector<uint32_t> data{first, second};

        stream.Put(gsl::make_span(data));

        REQUIRE(stream.Size() == expectedSize);
        REQUIRE(stream.Get<uint32_t>() == first);
        REQUIRE(stream.Get<uint32_t>() == second);
    }

    SECTION("MemoryStream::Shrink() must only remove data that has already been read")
    {
        size_t const expectedSize = 2;
        uint8_t const first = 3;
        uint8_t const second = 2;

        std::array<uint8_t, 4> data{8, 4, first, second};
        stream.Put(gsl::make_span(data));

        stream.Get<uint8_t>();
        stream.Get<uint8_t>();

        stream.Shrink();

        REQUIRE(stream.Size() == expectedSize);
        REQUIRE(stream.Get<uint8_t>() == first);
        REQUIRE(stream.Get<uint8_t>() == second);
    }

    SECTION("MemoryStream::Peek() must not modify the stream in any way")
    {
        size_t const expectedSize = 5;
        uint32_t const first = 1337;
        uint8_t const second = 255;

        stream.Put(first);
        stream.Put(second);

        REQUIRE(stream.Peek<uint8_t>(4) == second);

        REQUIRE(stream.Size() == expectedSize);
        REQUIRE(stream.Get<uint32_t>() == first);
        REQUIRE(stream.Get<uint8_t>() == second);
    }

    SECTION("MemoryStream::GetString() must return a valid std::string")
    {
        std::string const str = "Foobar";
        uint32_t const junk = 0xDEADBEEF;

        stream.Put(str);
        stream.Put(junk);

        REQUIRE(stream.GetString(str.size()) == str);
        REQUIRE(stream.Get<uint32_t>() == junk);
    }

    SECTION("Putting arrays on and retreiving them again from the stream")
    {
        using ElementType = int;
        size_t const NumElements = 4;
        using ArrayType = std::array<ElementType, NumElements>;

        ArrayType arr{ 1,2,3,4 };
        uint32_t const junk = 0xDEADBEEF;

        stream.Put(arr);
        stream.Put(junk);

        auto out = stream.Get<ElementType, NumElements>();

        for (int i = 0; i < NumElements; ++i)
            REQUIRE(arr[i] == out[i]);

        REQUIRE(stream.Get<uint32_t>() == junk);
    }
}
