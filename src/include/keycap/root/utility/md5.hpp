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

#include "../types.hpp"

#include <Botan/md5.h>

#include <array>

namespace keycap::root::network
{
    class memory_stream;
}

namespace keycap::root::utility
{
    namespace impl
    {
        template <typename T>
        void hash(Botan::HashFunction& md5, T const& value)
        {
            md5.update(value);
        }
    }

    template <typename... ARGS>
    std::array<uint8, 16> md5(ARGS&&... args)
    {
        thread_local auto md5 = Botan::HashFunction::create("MD5");

        md5->clear();
        (impl::hash(*md5, args), ...);

        std::array<uint8, 16> result;
        md5->final(result.data());
        return result;
    }
}