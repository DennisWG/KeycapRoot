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

#include <keycap/root/utility/memory.hpp>

#include <rapidcheck/catch.h>

#include <vector>

namespace util = keycap::root::utility;

TEST_CASE("Constructing array_views", "[array_view]")
{
    constexpr int arraySize = 6;
    std::vector<int> array{1, 2, 3, 4, 5, 6};
    util::array_view<int> view{array.data(), array.size()};

    SECTION("Constructing an array_view must return a valid array")
    {
        REQUIRE(view.size() == arraySize);
    }

    SECTION("Iterating over an array_view must yield the correct amount of iterations")
    {
        int count = 0;
        for (auto&& element : view)
            ++count;

        REQUIRE(count == arraySize);
    }

    SECTION("Accessing elements by valid index must return the correct element")
    {
        for (int i = 0; i < arraySize; ++i)
            REQUIRE(array[i] == view[i]);
    }

    SECTION("Accessing elements by invalid index must return the correct element")
    {
        REQUIRE_THROWS(view[-1]);
        REQUIRE_THROWS(view[arraySize]);
    }

    SECTION("No memory is being copied!")
    {
        REQUIRE(view.data() == array.data());
    }
}