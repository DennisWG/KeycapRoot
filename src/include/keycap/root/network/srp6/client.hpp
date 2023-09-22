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

#include "compliance.hpp"
#include "group_parameters.hpp"

#include <botan_all.h>

namespace keycap::root::network::srp6
{
    constexpr int CLIENT_SECRET_BTIS = 32;
    class client final
    {
      public:
        client(
            group_parameter groupParameter, std::string username, compliance compliance,
            Botan::BigInt const& a = Botan::BigInt::decode(Botan::AutoSeeded_RNG().random_vec(CLIENT_SECRET_BTIS)));

        // Returns the public ephemeral value (A)
        Botan::BigInt const& public_ephemeral_value() const noexcept;

        // Returns the prime (N)
        Botan::BigInt const& prime() const noexcept;

        // Returns the generator (g)
        Botan::BigInt const& generator() const noexcept;

        // Returns the session key (K = H(S))
        Botan::BigInt session_key(
            Botan::BigInt const& B, std::string const& username, std::string const& password,
            Botan::BigInt const& salt);

      private:
        compliance compliance_;

        const Botan::BigInt N_;
        const Botan::BigInt g_;

        std::string I_;
        Botan::BigInt a_;
        Botan::BigInt A_;
    };
}