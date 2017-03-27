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

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>


namespace Keycap::Root::Utility
{
    namespace impl
    {
        using nameValueMap_t = std::unordered_map<std::string, int32_t>;
        using valueNameMap_t = std::unordered_map<int32_t, std::string>;

        std::string removeWhitespaces(std::string str)
        {
            str.erase(std::remove_if(str.begin(), str.end(), [](char c)
            {
                return c == ' ' || c == '\t';
            }), str.end());

            return str;
        }

        bool convert(std::string const& str, int32_t& outInt)
        {
            std::stringstream ss;
            ss << str;

            if (ss >> outInt)
                return true;

            return false;
        }

        std::string extractEntry(std::string& values)
        {
            std::string result;
            auto itr = values.find(',');

            if (itr != std::string::npos)
            {
                auto entry = values.substr(0, itr);
                values.erase(0, itr + 1);

                result = removeWhitespaces(entry);
            }
            else
            {
                result = removeWhitespaces(values);
                values.clear();
            }

            return result;
        }

        std::optional<std::string> extractValue(std::string& values)
        {
            std::optional<std::string> ret;

            auto itr = values.find('=');
            if (itr != std::string::npos)
            {
                ret = values.substr(itr + 1);
                values.erase(itr);
            }

            return ret;
        }

        bool isPowerOfTwoOrZero(int32_t number)
        {
            return (number & (number - 1)) == 0;
        }

        template <typename Callback>
        std::pair<int32_t, int32_t> doForEachEntry(std::string& values, nameValueMap_t& map, bool fix, Callback const& callback)
        {
            int32_t currentValue = 0;
            int32_t minValue = std::numeric_limits<int32_t>::max();

            while (!values.empty())
            {
                auto entry = extractEntry(values);
                auto value = extractValue(entry);

                if (value)
                {
                    int32_t intValue;
                    if (convert(*value, intValue))
                        currentValue = intValue;
                    else if (fix)
                        currentValue = map[*value];
                    else
                        continue;
                }

                minValue = std::min(minValue, currentValue);
                callback(entry, currentValue);

                ++currentValue;
            }

            return std::make_pair(minValue, currentValue);
        }

        nameValueMap_t makeNameValueMap(std::string values)
        {
            nameValueMap_t map;

            doForEachEntry(values, map, false, [&](std::string const& entry, int32_t value)
            {
                map[entry] = value;
            });

            return map;
        }

        valueNameMap_t makeValueNameMap(std::string values, nameValueMap_t& lookup, bool fillMissing = false)
        {
            valueNameMap_t map;

            auto minmax = doForEachEntry(values, lookup, false, [&](std::string const& entry, int32_t value)
            {
                if(map.find(value) == map.end())
                    map[value] = entry;
            });

            if (fillMissing)
            {
                auto minValue = minmax.first;
                auto maxValue = minmax.second * 2;

                for (int32_t i = minValue; i < maxValue; ++i)
                {
                    if (isPowerOfTwoOrZero(i))
                        continue;

                    if (map.find(i) != map.end())
                        continue;

                    const size_t numBits = CHAR_BIT * sizeof(int32_t);
                    std::bitset<numBits> bits = i;
                    bool first = true;

                    for (int32_t n = 0; n < numBits; ++n)
                    {
                        if (!bits.test(n))
                            continue;

                        auto key = 1 << n;
                        auto itr = map.find(key);
                        if (itr != map.end())
                        {
                            if (first)
                            {
                                map[i] = itr->second;
                                first = false;
                            }
                            else
                                map[i] = map[i] + " | " + itr->second;
                        }
                    }
                }
            }

            return map;
        }

        template<typename ElementType>
        std::vector<ElementType> makeVector(std::string values, nameValueMap_t& lookup, valueNameMap_t& valueLookup)
        {
            std::vector<ElementType> vector;

            doForEachEntry(values, lookup, false, [&](std::string const& entry, int32_t value)
            {
                auto element = static_cast<ElementType>(value);

                if (std::find(vector.begin(), vector.end(), element) == vector.end())
                    vector.push_back(element);

            });

            return vector;
        }
    }
}

// clang-format off
#define keycap_enum(Type, ...) \
std::unordered_map<std::string, int32_t> Type##_name_value_map = Keycap::Root::Utility::impl::makeNameValueMap(#__VA_ARGS__); \
std::unordered_map<int32_t, std::string> Type##_value_name_map = Keycap::Root::Utility::impl::makeValueNameMap(#__VA_ARGS__, Type##_name_value_map, false); \
class Type final { \
public: \
    enum Enum { __VA_ARGS__ Max }; \
    Type() = default; \
    Type(Enum value) : value_{value} {} \
    explicit Type (int32_t value) : value_{static_cast<Enum>(value)} {} \
    Type& operator=(Enum value) { value_ = value; return *this; } \
    bool operator==(Type const& rhs) { return value_ == rhs.value_; } \
    Enum Get() const { return value_; } \
    void Set(Enum value) { value_ = value; } \
    std::string ToString() const { return Type##_value_name_map[static_cast<int32_t>(value_)]; } \
    static std::vector<Type> const& ToVector() { static auto vector = Keycap::Root::Utility::impl::makeVector<Type>(#__VA_ARGS__, Type##_name_value_map, Type##_value_name_map); return vector; } \
private: \
    Enum value_; \
}

#define keycap_enum_flags(Type, ...) \
std::unordered_map<std::string, int32_t> Type##_name_value_map = Keycap::Root::Utility::impl::makeNameValueMap(#__VA_ARGS__); \
std::unordered_map<int32_t, std::string> Type##_value_name_map = Keycap::Root::Utility::impl::makeValueNameMap(#__VA_ARGS__, Type##_name_value_map, true); \
class Type final { \
public: \
    enum Enum { __VA_ARGS__ Max }; \
    Type() = default; \
    Type(Enum value) : value_{value} {} \
    explicit Type (int32_t value) : value_{static_cast<Enum>(value)} {} \
    Type& operator=(Enum value) { value_ = value; return *this; } \
    bool operator==(Type const& rhs) { return value_ == rhs.value_; } \
    void SetFlag(Enum which) { value_ = static_cast<Enum>(static_cast<int32_t>(value_) | which); } \
    void SetAllFlags() { value_ = static_cast<Enum>((static_cast<int32_t>(Max) - 1) * 2 - 1); } \
    void ClearFlag(Enum which) { value_ = static_cast<Enum>(static_cast<int32_t>(value_) & ~which); } \
    void ClearAllFlags() { value_ = static_cast<Enum>(0); } \
    void ToggleFlag(Enum which) { value_ = static_cast<Enum>(static_cast<int32_t>(value_) ^ which); } \
    bool TestFlag(Enum which) const { return static_cast<Enum>(static_cast<int32_t>(value_) & which) == which; } \
    Enum Get() const { return value_; } \
    void Set(Enum value) { value_ = value; } \
    std::string ToString() const { return Type##_value_name_map[static_cast<int32_t>(value_)]; } \
    static std::vector<Type> const& ToVector() { static auto vector = Keycap::Root::Utility::impl::makeVector<Type>(#__VA_ARGS__, Type##_name_value_map, Type##_value_name_map); return vector; } \
private: \
    Enum value_; \
}
// clang-format on
