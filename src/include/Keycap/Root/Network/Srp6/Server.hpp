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

#include "Compliance.hpp"
#include "GroupParameters.hpp"

#include <botan/bigint.h>

namespace Keycap::Root::Network::Srp6
{
    class Server final
    {
      public:
        Server(GroupParameter groupParameter, Botan::BigInt const& v, Compliance compliance, std::size_t secretBits = 32);

        // Returns the public ephemeral value (B)
        Botan::BigInt const& PublicEphemeralValue() const;

        // Returns the session key (H(S))
        std::vector<uint8_t> SessionKey(Botan::BigInt const& A);

      private:
        Server(Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& v, Botan::BigInt const& b, Compliance compliance);

        // Prime
        const Botan::BigInt N_;

        // Generator
        const Botan::BigInt g_;

        // Password verifier
        const Botan::BigInt v_;

        // Private ephemeral value
        const Botan::BigInt b_;

        // Public ephemeral values
        Botan::BigInt B_;
        Botan::BigInt A_;

        // Compliance mode
        Compliance compliance_;
    };
}