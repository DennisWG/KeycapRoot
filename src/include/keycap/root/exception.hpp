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

#include <exception>
#include <format>
#include <stacktrace>
#include <string>

namespace keycap
{
    class exception : public std::exception
    {
        exception(std::string msg, std::stacktrace const& trace)
        {
            _msg = std::format("{}:\n{}", msg, std::to_string(trace));
        }

      public:
        exception(std::string const& message)
          : exception(message, std::stacktrace::current())
        {
        }

        exception(char const* message)
          : exception(message, std::stacktrace::current())
        {
        }

        [[nodiscard]] virtual char const* what() const
        {
            return _msg.c_str();
        }

      private:
        std::string _msg;
        std::stacktrace _trace;
    };
}