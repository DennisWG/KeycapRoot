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

#include <Keycap/Root/Utility/Utility.hpp>
#include <rapidcheck/catch.h>
#include <spdlog/spdlog.h>

TEST_CASE("Utility")
{
    namespace util = Keycap::Root::Utility;

    rc::prop("GetSafeLogger should never return nullptr", [](std::string const& name) {
        auto logger = util::GetSafeLogger(name);
        REQUIRE(logger != nullptr);
    });
}