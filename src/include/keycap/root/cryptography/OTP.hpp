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

#include <array>
#include <string>

namespace keycap::root::cryptography
{
    enum class foo
    {
    };

    namespace HOTP
    {
        // Implementation of the OATH HMAC-based One-Time Password algorithm
        // See: https://tools.ietf.org/html/rfc4226
        [[nodiscard]] std::string generate(std::string const& key, uint64_t counter, size_t num_digits = 6);
    }

    namespace TOTP
    {
        // Implementation of the OATH Time-based One-Time Password algorithm
        // See: https://tools.ietf.org/html/rfc6238
        [[nodiscard]] std::string
        generate(std::string const& key, time_t now, time_t start, time_t step, size_t num_digits = 6);

        // Validates the given code for the given secret
        [[nodiscard]] bool validate(std::string const& key, std::string const& code);
    };
}