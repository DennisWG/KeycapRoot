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

#include <Keycap/Root/configuration/config_file.hpp>
#include <rapidcheck/catch.h>

namespace config = keycap::root::configuration;

TEST_CASE("Constructing a config_file", "[config_file]")
{
    SECTION("Given a valid file path with valid json, constructing a config_file should not throw")
    {
        REQUIRE_NOTHROW(config::config_file{"test.cfg"});
    }

    SECTION("Given a valid file path with invalid json, constructing a config_file should throw")
    {
        REQUIRE_THROWS(config::config_file{"invalid.cfg"});
    }

    SECTION("Given an invalid file path, constructing a config_file should throw")
    {
        REQUIRE_THROWS(config::config_file{"invalid/File/path.cfg"});
    }
}

TEST_CASE("Testing config_file::get", "[config_file]")
{
    config::config_file cfgFile{"test.cfg"};

    SECTION("config_file::get must return the requested value if both the category and the value are valid")
    {
        auto ip = cfgFile.get<std::string>("Network", "IP");
        REQUIRE(ip == "127.0.0.1");

        auto port = cfgFile.get<int>("Network", "Port");
        REQUIRE(port == 12345);

        auto something = cfgFile.get<bool>("Network", "Something");
        REQUIRE(something == true);
    }

    SECTION("config_file::get must check for values outside of categories if no category can be found")
    {
        auto value = cfgFile.get<bool>("", "NoCategory");
        REQUIRE(value == false);
    }

    SECTION("config_file::get must throw an exception if attempting to retreive a value from empty category")
    {
        REQUIRE_THROWS(cfgFile.get<int>("Empty", "Invalid"));
    }

    SECTION("config_file::get must throw an excception if attempting to retreive non existant value from category")
    {
        REQUIRE_THROWS(cfgFile.get<int>("Network", "Invalid"));
    }

    SECTION("config_file::get must throw an exception if attempting to retreive non existant value outside of category")
    {
        REQUIRE_THROWS(cfgFile.get<int>("", "Invalid"));
    }
}

TEST_CASE("Testing config_file::get_or_default", "[config_file]")
{
    config::config_file cfgFile{"test.cfg"};

    SECTION("config_file::get_or_default must return the requested value if both the category and the value are valid")
    {
        auto ip = cfgFile.get_or_default<std::string>("Network", "IP", "");
        REQUIRE(ip == "127.0.0.1");

        auto port = cfgFile.get_or_default<int>("Network", "Port", 0);
        REQUIRE(port == 12345);

        auto something = cfgFile.get_or_default<bool>("Network", "Something", false);
        REQUIRE(something == true);
    }

    SECTION("config_file::get_or_default must return matching values outside of categories if no category can be found")
    {
        auto value = cfgFile.get_or_default<bool>("", "NoCategory", true);
        REQUIRE(value == false);
    }

    SECTION(
        "config_file::get_or_default must return the given default value if attempting to retreive a value from empty "
        "category")
    {
        int const defaultValue = 0;
        auto value = cfgFile.get_or_default<int>("Empty", "Invalid", defaultValue);
        REQUIRE(value == defaultValue);
    }

    SECTION(
        "config_file::get_or_default must return the given default value if attempting to retreive non existant value "
        "from category")
    {
        std::string const defaultValue = "-";
        auto value = cfgFile.get_or_default<std::string>("Network", "Invalid", defaultValue);
        REQUIRE(value == defaultValue);
    }

    SECTION(
        "config_file::get_or_default must return the given default value if attempting to retreive non existant value "
        "outside of category")
    {
        bool const defaultValue = true;
        auto value = cfgFile.get_or_default<bool>("", "Invalid", defaultValue);
        REQUIRE(value == defaultValue);
    }
}

TEST_CASE("Testing config_file::iterate_array", "[config_file]")
{
    config::config_file cfgFile{"test.cfg"};

    std::vector<std::pair<std::string, int>> expected_values = {{"Huey", 21}, {"Marry", 25}, {"Hans", 18}};
    auto current_values = expected_values.begin();

    cfgFile.iterate_array("", "Array", [&current_values](config::config_entry& value) {
        auto name = value.get_or_default<std::string>("", "name", "no-name");
        auto age = value.get_or_default("", "age", 21);

        auto[required_name, required_age] = *current_values;
        ++current_values;

        REQUIRE(name == required_name);
        REQUIRE(age == required_age);
    });
}