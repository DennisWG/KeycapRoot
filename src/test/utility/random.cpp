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

#include <keycap/root/utility/random.hpp>

#include <rapidcheck/catch.h>

TEST_CASE("random")
{
    namespace util = keycap::root::utility;

    rc::prop(
        "generating a uint16_t with min and max constraints must yield a random number within the given constrains",
        [](uint16_t min, uint16_t max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_ui16(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a uint32_t with min and max constraints must yield a random number within the given constrains",
        [](uint32_t min, uint32_t max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_ui32(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a uint64_t with min and max constraints must yield a random number within the given constrains",
        [](uint64_t min, uint64_t max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_ui64(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a int16_t with min and max constraints must yield a random number within the given constrains",
        [](int16_t min, int16_t max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_i16(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a int32_t with min and max constraints must yield a random number within the given constrains",
        [](int32_t min, int32_t max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_i32(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a int64_t with min and max constraints must yield a random number within the given constrains",
        [](int64_t min, int64_t max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_i64(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a float with min and max constraints must yield a random number within the given constrains",
        [](float min, float max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_float(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });

    rc::prop(
        "generating a double with min and max constraints must yield a random number within the given constrains",
        [](double min, double max) {
            if (min > max)
                std::swap(min, max);

            auto value = util::random_double(min, max);

            REQUIRE(min <= value);
            REQUIRE(value <= max);
        });
}