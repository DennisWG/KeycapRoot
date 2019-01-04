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

#include <boost/lexical_cast/try_lexical_convert.hpp>

namespace keycap
{
    template <typename T1, typename T2>
    T1 lexical_cast(T2&& value, T1&& default_value)
    {
        T1 ret_value;

        if constexpr (std::is_pointer_v<T2>)
        {
            if (value == nullptr)
                return default_value;
        }

        if (boost::conversion::try_lexical_convert(std::forward<T2>(value), ret_value))
            return ret_value;

        return std::forward<T1>(default_value);
    }
}