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

namespace Keycap::Root
{
    template <typename T>
    inline auto array_index(T const& element, T const& begin) noexcept
    {
        return &element - &begin;
    }

    template <typename T>
    inline auto is_odd(T const& value) noexcept
    {
        return (value % 2) == 1;
    }

    template <typename T>
    inline auto is_even(T const& value) noexcept
    {
        return !is_odd(value);
    }
}