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

#include <botan/auto_rng.h>
#include <botan/bigint.h>

namespace keycap::root::network::srp6
{
    constexpr int SECRET_BTIS = 32;
    class server final
    {
      public:
        server(
            group_parameter groupParameter, Botan::BigInt const& v, compliance compliance,
            Botan::BigInt const& b = Botan::BigInt::decode(Botan::AutoSeeded_RNG().random_vec(SECRET_BTIS)));

        // Returns the public ephemeral value (B)
        Botan::BigInt const& public_ephemeral_value() const noexcept;

        // Returns the session key (K = H(S))
        Botan::BigInt session_key(Botan::BigInt const& A);

        // Generates the server proof (M2_S)
        Botan::BigInt proof(Botan::BigInt const& clientProof, Botan::BigInt const& sessionKey) const;

        // Returns the Compliance mode
        compliance compliance_mode() const noexcept;

        // Returns the prime
        Botan::BigInt const& prime() const noexcept;

        // Returns the generator
        Botan::BigInt const& generator() const noexcept;

        // Returns the verifier (v)
        Botan::BigInt const& verifier() const noexcept;

      private:
        server(
            Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& v, Botan::BigInt const& b,
            compliance compliance);

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
        compliance compliance_;
    };
}
