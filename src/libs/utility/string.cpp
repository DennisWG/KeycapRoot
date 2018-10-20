/*
    Copyright 2018 KeycapEmu

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

#include <keycap/root/utility/string.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>

namespace keycap::root::utility
{
    std::vector<std::string> explode(std::string const& str, char delim)
    {
        std::vector<std::string> result;
        std::istringstream ss(str);

        for (std::string token; std::getline(ss, token, delim);)
            result.push_back(std::move(token));

        return result;
    }

    std::string join(std::vector<std::string> const& vec, char delim)
    {
        std::string str;
        str.reserve(256);

        bool first = true;

        for (auto const& s : vec)
        {
            if (!first)
                str += delim;

            str += s;

            first = false;
        }

        return str;
    }

    std::string to_upper(std::string s)
    {
        [[maybe_unused]] auto end
            = std::transform(std::begin(s), std::end(s), std::begin(s), [](auto c) { return std::toupper(c); });
        return s;
    }

    std::string to_lower(std::string s)
    {
        [[maybe_unused]] auto end
            = std::transform(std::begin(s), std::end(s), std::begin(s), [](auto c) { return std::tolower(c); });
        return s;
    }
}