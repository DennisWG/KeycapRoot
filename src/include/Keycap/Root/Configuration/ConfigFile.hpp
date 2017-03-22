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

#include <experimental/filesystem>
#include <json.hpp>

#include <string>
#include <fstream>


#include <iostream>

namespace Keycap::Root::Configuration
{
    // Manages configuration data stored on file
    class ConfigFile
    {
    public:
        // Opens and parsed the file at the given filePath
        ConfigFile(std::experimental::filesystem::path const& filePath)
        {
            std::ifstream file(filePath.string());
            file >> json_;
        }

        // Attempts to retreive the value of the given category with the given name. Throws an exception if fails to do so
        template <typename ReturnType>
        ReturnType Get(std::string const& category, std::string const& value) const
        {
            auto itr = json_.find(category);
            if (itr != json_.end())
            {
                if (!itr->is_object())
                    throw std::exception{(category + " does not contain any items!").c_str()};

                auto valueItr = itr->find(value);
                if (valueItr == itr->end())
                {
                    auto msg = ("Category " + category + " does not contain item " + value).c_str();
                    throw std::exception{ msg };
                }

                return valueItr->get<ReturnType>();
            }
            else
            {
                itr = json_.find(value);
                if (itr != json_.end())
                    return itr->get<ReturnType>();
            }

            throw std::exception{ ("Could not find standalone key " + category + " " + value).c_str() };
        }

        // Attempts to retreive the value of the given category with the given name. Returns the given default if it fails to do so
        template <typename ReturnType>
        ReturnType GetOrDefault(std::string const& category, std::string const& value, ReturnType default) const
        {
            auto itr = json_.find(category);
            if (itr != json_.end())
            {
                if (!itr->is_object())
                    return default;

                auto valueItr = itr->find(value);
                if (valueItr == itr->end())
                    return default;

                return valueItr->get<ReturnType>();
            }
            else
            {
                itr = json_.find(value);
                if (itr != json_.end())
                    return itr->get<ReturnType>();
            }

            return default;
        }

    private:
        nlohmann::json json_;
    };
}