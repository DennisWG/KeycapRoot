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

#include <string>

namespace Keycap::Root::Network::Srp6
{
    // Group Parameters taken from https://tools.ietf.org/html/rfc5054#page-16

    struct GroupParameter
    {
        std::string value;
        uint64_t generator;
    };

    enum class GroupParameters
    {
        _256,
        _1024,
        _1536,
        _2048,
        _3072,
        _4096,
        _6144,
        _8192,
    };

    GroupParameter GetParameters(GroupParameters parameters);
}