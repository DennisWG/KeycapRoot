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

#include <Keycap/Root/Utility/Enum.hpp>

#include <rapidcheck/catch.h>

keycap_enum(SomeEnum, int,
    Entry1,
    Entry2 = Entry1,
    Entry3,
    Entry4 = -2,
);

TEST_CASE("Iterating enums", "[keycap_enum]")
{
    SECTION("String representation of values should be exactly like the ones listed while skipping aliases")
    {
        static std::vector<std::string> expectedNames {"Entry1", "Entry3", "Entry4"};

        auto values = SomeEnum::ToVector();

        REQUIRE(values.size() == expectedNames.size());

        for (size_t i = 0; i < values.size(); ++i)
            REQUIRE(values[i].ToString() == expectedNames[i]);
    }

    SECTION("Elements should auto deduce values starting from 0 if no value is explicitly stated")
    {
        static std::vector<int32_t> expectedValues {0, 1, -2};

        auto values = SomeEnum::ToVector();

        REQUIRE(values.size() == expectedValues.size());

        for (size_t i = 0; i < values.size(); ++i)
            REQUIRE(values[i].Get() == expectedValues[i]);
    }

    SECTION("Setting the given value")
    {
        SomeEnum e;

        e.Set(SomeEnum::Entry4);
        REQUIRE(e.Get() == SomeEnum::Entry4);

        e = SomeEnum::Entry3;
        REQUIRE(e.Get() == SomeEnum::Entry3);
    }
}

keycap_enum_flags(SomeFlags, int,
    None = 0,
    Red = 1,
    Green = 2,
    Blue = 4,
);

TEST_CASE("Iterating enum flags", "[keycap_enum_flags]")
{
    SECTION("String representation of values should be exactly like the ones listed while skipping aliases")
    {
        static std::vector<std::string> expectedNames{ "None", "Red", "Green", "Blue" };

        auto values = SomeFlags::ToVector();

        REQUIRE(values.size() == expectedNames.size());

        for (size_t i = 0; i < values.size(); ++i)
            REQUIRE(values[i].ToString() == expectedNames[i]);
    }

    SECTION("Elements should auto deduce values starting from 0 if no value is explicitly stated")
    {
        static std::vector<int32_t> expectedValues{ 0, 1, 2, 4 };

        auto values = SomeFlags::ToVector();

        REQUIRE(values.size() == expectedValues.size());

        for (size_t i = 0; i < values.size(); ++i)
            REQUIRE(values[i].Get() == expectedValues[i]);
    }

    SECTION("Setting the given value")
    {
        SomeFlags e;

        e.Set(SomeFlags::Red);
        REQUIRE(e.Get() == SomeFlags::Red);

        e = SomeFlags::None;
        REQUIRE(e.Get() == SomeFlags::None);
    }

    SECTION("Setting, unsetting and testing flags")
    {
        SomeFlags flags = SomeFlags::Red;

        flags.SetFlag(SomeFlags::Blue);
        REQUIRE(flags.TestFlag(SomeFlags::Blue));
        REQUIRE(flags.TestFlag(SomeFlags::Red));
        REQUIRE(flags.TestFlag(SomeFlags::Green) == false);

        flags.ClearAllFlags();
        REQUIRE(flags.Get() == 0);

        flags.SetAllFlags();
        REQUIRE(flags.TestFlag(SomeFlags::Blue));
        REQUIRE(flags.TestFlag(SomeFlags::Red));
        REQUIRE(flags.TestFlag(SomeFlags::Green));

        flags.ClearFlag(SomeFlags::Green);
        REQUIRE(flags.TestFlag(SomeFlags::Blue));
        REQUIRE(flags.TestFlag(SomeFlags::Red));
        REQUIRE(flags.TestFlag(SomeFlags::Green) == false);

        flags.ToggleFlag(SomeFlags::Red);
        REQUIRE(flags.TestFlag(SomeFlags::Blue));
        REQUIRE(flags.TestFlag(SomeFlags::Red) == false);
        REQUIRE(flags.TestFlag(SomeFlags::Green) == false);
    }
}