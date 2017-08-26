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

#pragma once

#include <memory>
#include <string>

namespace spdlog
{
    class logger;
}

namespace Keycap::Root::Utility
{
    // Sets the console's title to the given title
    extern void SetConsoleTitle(std::string const& title);

    // Returns the logger of the given name if it exists or returns a newly created one with a null_sink
    extern std::shared_ptr<spdlog::logger> GetSafeLogger(std::string const& name);

    template <typename T>
    auto shared_from_that(T* p)
    {
        return std::shared_ptr<T>(p->shared_from_this(), p);
    }
}