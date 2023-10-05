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

#include "../utility/string.hpp"

#include <keycap/root/exception.hpp>

#include <filesystem>
#include <nlohmann/json.hpp>

#include <fstream>
#include <string>

namespace keycap::root::configuration
{
    // Represents an arbitrary entry withing the config file
    class config_entry
    {
      public:
        config_entry(nlohmann::json::value_type const& value)
          : json_{value}
        {
        }

        // Attempts to retreive the value of the given category with the given name. Throws an exception if fails to do
        // so
        template <typename ReturnType>
        ReturnType get(std::string const& category, std::string const& value) const
        {
            auto itr = json_.find(category);
            if (itr != json_.end())
            {
                if (!itr->is_object())
                {
                    throw exception{category + " does not contain any items!"};
                }

                auto value_itr = itr->find(value);
                if (value_itr == itr->end())
                {
                    auto msg = "Category " + category + " does not contain item " + value;
                    throw exception{msg};
                }

                return value_itr->get<ReturnType>();
            }
            else
            {
                itr = json_.find(value);
                if (itr != json_.end())
                    return itr->get<ReturnType>();
            }

            throw exception{"Could not find standalone key " + category + " " + value};
        }

        // Attempts to retreive the value of the given category with the given name. Returns the given default if it
        // fails to do so
        template <typename ReturnType>
        ReturnType get_or_default(std::string const& category, std::string const& value, ReturnType default_value) const
        {
            auto itr = json_.find(category);
            if (itr != json_.end())
            {
                if (!itr->is_object())
                    return default_value;

                auto value_itr = itr->find(value);
                if (value_itr == itr->end())
                    return default_value;

                return value_itr->get<ReturnType>();
            }
            else
            {
                itr = json_.find(value);
                if (itr != json_.end())
                    return itr->get<ReturnType>();
            }

            return default_value;
        }

      private:
        nlohmann::json::value_type const& json_;
    };

    // Manages configuration data stored on file
    class config_file
    {
      public:
        // Opens and parsed the file at the given filePath
        config_file(std::filesystem::path const& filePath);

        // Attempts to retreive the value of the given category with the given name. Throws an exception if fails to do
        // so
        template <typename ReturnType>
        ReturnType get(std::string const& category, std::string const& value) const
        {
            auto itr = json_.find(category);
            if (itr != json_.end())
            {
                if (!itr->is_object())
                    throw exception{category + " does not contain any items!"};

                auto value_itr = itr->find(value);
                if (value_itr == itr->end())
                {
                    auto msg = "Category " + category + " does not contain item " + value;
                    throw exception{msg};
                }

                return value_itr->get<ReturnType>();
            }
            else
            {
                itr = json_.find(value);
                if (itr != json_.end())
                    return itr->get<ReturnType>();
            }

            throw exception{"Could not find standalone key " + category + " " + value};
        }

        // Attempts to retreive the value of the given category with the given name. Returns the given default if it
        // fails to do so
        template <typename ReturnType>
        ReturnType get_or_default(std::string const& category, std::string const& value, ReturnType default_value) const
        {
            auto itr = json_.find(category);
            if (itr != json_.end())
            {
                if (!itr->is_object())
                    return default_value;

                auto value_itr = itr->find(value);
                if (value_itr == itr->end())
                    return default_value;

                return value_itr->get<ReturnType>();
            }
            else
            {
                itr = json_.find(value);
                if (itr != json_.end())
                    return itr->get<ReturnType>();
            }

            return default_value;
        }

        template <typename Callback>
        void iterate_array(std::string const& category, std::string const& value, Callback&& callback) const
        {
            auto iterate = [callback = std::forward<Callback>(callback)](nlohmann::json::value_type const& value)
            {
                for (auto& i : value)
                    callback(config_entry{i});
            };

            if (auto itr = json_.find(category); itr != json_.end())
            {
                if (!itr->is_object())
                    return;

                auto value_itr = itr->find(value);
                if (value_itr == itr->end() || !value_itr->is_array())
                    return;

                iterate(*value_itr);
            }
            else
            {
                itr = json_.find(value);
                if (itr == json_.end() || !itr->is_array())
                    return;

                iterate(*itr);
            }
        }

      private:
        nlohmann::json json_;
    };
}