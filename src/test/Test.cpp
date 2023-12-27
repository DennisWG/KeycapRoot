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
#include <Keycap/Root/Utility/Utility.hpp>
#include <rapidcheck/catch.h>

#include <Keycap/Root/Network/Service.hpp>

#define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include <Keycap/Root/Network/MemoryStream.hpp>
#include <Keycap/Root/Network/Srp6/Server.hpp>
#include <gsl/span>

#ifdef _WIN32
#undef SetConsoleTitle
#endif

struct Test
{
    std::string name;
    std::string address;
    int age;
};

void to_json(nlohmann::json& j, Test const& t)
{
    j = nlohmann::json{{"name", t.name}, {"address", t.address}, {"age", t.age}};
}

void from_json(nlohmann::json const& j, Test& t)
{
    if (j.find("awdnawjdnawdndw") != j.end())
        auto foo = j["akwdnkawndawd"];
    t.name = j["name"].get<std::string>();
    t.address = j["address"].get<std::string>();
    t.age = j["age"].get<int>();
}

TEST_CASE("FooBar")
{
    namespace util = keycap::root::Utility;

    util::SetConsoleTitle("Test foobar");
    auto logger = util::GetSafeLogger("testing");

    logger->info("Foobar");

    // keycap::root::configuration::ConfigFile<int> file("../test.cfg.test");
    // file.Foo();

    SECTION("json")
    {
        Test t{"Peter", "Barsch", 44};

        nlohmann::json j = t;

        Test m = j;

        // std::cout << j << "\n";
    }

    Botan::Bigint int2;

    // REQUIRE(false);
}