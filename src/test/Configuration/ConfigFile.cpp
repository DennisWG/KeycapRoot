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

#include <Keycap/Root/Configuration/ConfigFile.hpp>
#include <rapidcheck/catch.h>

namespace config = Keycap::Root::Configuration;

TEST_CASE("Constructing a ConfigFile", "[ConfigFile]")
{
    SECTION("Given a valid file path with valid json, constructing a ConfigFile should not throw")
    {
        REQUIRE_NOTHROW(config::ConfigFile{"../test.cfg"});
    }

    SECTION("Given a valid file path with invalid json, constructing a ConfigFile should throw")
    {
        REQUIRE_THROWS(config::ConfigFile{"../invalid.cfg"});
    }

    SECTION("Given an invalid file path, constructing a ConfigFile should throw")
    {
        REQUIRE_THROWS(config::ConfigFile{"invalid/File/path.cfg"});
    }
}

TEST_CASE("Testing ConfigFile::Get", "[ConfigFile]")
{
    config::ConfigFile cfgFile{"../test.cfg"};

    SECTION("ConfigFile::Get must return the requested value if both the category and the value are valid")
    {
        auto ip = cfgFile.Get<std::string>("Network", "IP");
        REQUIRE(ip == "127.0.0.1");

        auto port = cfgFile.Get<int>("Network", "Port");
        REQUIRE(port == 12345);

        auto something = cfgFile.Get<bool>("Network", "Something");
        REQUIRE(something == true);
    }

    SECTION("ConfigFile::Get must check for values outside of categories if no category can be found")
    {
        auto value = cfgFile.Get<bool>("", "NoCategory");
        REQUIRE(value == false);
    }

    SECTION("ConfigFile::Get must throw an exception if attempting to retreive a value from empty category")
    {
        REQUIRE_THROWS(cfgFile.Get<int>("Empty", "Invalid"));
    }

    SECTION("ConfigFile::Get must throw an excception if attempting to retreive non existant value from category")
    {
        REQUIRE_THROWS(cfgFile.Get<int>("Network", "Invalid"));
    }

    SECTION("ConfigFile::Get must throw an exception if attempting to retreive non existant value outside of category")
    {
        REQUIRE_THROWS(cfgFile.Get<int>("", "Invalid"));
    }
}

TEST_CASE("Testing ConfigFile::GetOrDefault", "[ConfigFile]")
{
    config::ConfigFile cfgFile{"../test.cfg"};

    SECTION("ConfigFile::GetOrDefault must return the requested value if both the category and the value are valid")
    {
        auto ip = cfgFile.GetOrDefault<std::string>("Network", "IP", "");
        REQUIRE(ip == "127.0.0.1");

        auto port = cfgFile.GetOrDefault<int>("Network", "Port", 0);
        REQUIRE(port == 12345);

        auto something = cfgFile.GetOrDefault<bool>("Network", "Something", false);
        REQUIRE(something == true);
    }

    SECTION("ConfigFile::GetOrDefault must return matching values outside of categories if no category can be found")
    {
        auto value = cfgFile.GetOrDefault<bool>("", "NoCategory", true);
        REQUIRE(value == false);
    }

    SECTION("ConfigFile::GetOrDefault must return the given default value if attempting to retreive a value from empty "
            "category")
    {
        int const defaultValue = 0;
        auto value = cfgFile.GetOrDefault<int>("Empty", "Invalid", defaultValue);
        REQUIRE(value == defaultValue);
    }

    SECTION("ConfigFile::GetOrDefault must return the given default value if attempting to retreive non existant value "
            "from category")
    {
        std::string const defaultValue = "-";
        auto value = cfgFile.GetOrDefault<std::string>("Network", "Invalid", defaultValue);
        REQUIRE(value == defaultValue);
    }

    SECTION("ConfigFile::GetOrDefault must return the given default value if attempting to retreive non existant value "
            "outside of category")
    {
        bool const defaultValue = true;
        auto value = cfgFile.GetOrDefault<bool>("", "Invalid", defaultValue);
        REQUIRE(value == defaultValue);
    }
}
