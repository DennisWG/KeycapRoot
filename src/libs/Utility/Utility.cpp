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
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define NOMINMAX

#include <Windows.h>

#undef SetConsoleTitle
#endif

namespace Keycap::Root::Utility
{
    void SetConsoleTitle(std::string const& title) noexcept
    {
#ifdef _WIN32
        SetConsoleTitleA(title.c_str());
#elif defined __linux__ || defined __unix__
        std::cout << "\033]0;" << title << "}\007";
#endif
    }

    std::shared_ptr<spdlog::logger> GetSafeLogger(std::string const& name)
    {
        auto logger = spdlog::get(name);
        if (!logger)
        {
            auto null_sink = std::make_shared<spdlog::sinks::null_sink_st>();
            return std::make_shared<spdlog::logger>("null_logger", null_sink);
        }

        return logger;
    }
}

#ifdef _WIN32
#undef WIN32_LEAN_AND_MEAN
#undef VC_EXTRALEAN
#undef NOMINMAX
#endif