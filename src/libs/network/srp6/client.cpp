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

#include "utility.hpp"

#include <keycap/root/network/srp6/client.hpp>
#include <keycap/root/network/srp6/utility.hpp>

#include <botan/numthry.h>
#include <botan/sha160.h>

namespace keycap::root::network::srp6
{
    client::client(group_parameter groupParameter, std::string username, compliance compliance, Botan::BigInt const& a)
      : compliance_{compliance}
      , N_{groupParameter.N}
      , g_{groupParameter.g}
      , I_{username}
      , a_{a}
      , A_{Botan::power_mod(g_, a, N_)}
    {
    }

    Botan::BigInt const& client::public_ephemeral_value() const noexcept
    {
        return A_;
    }

    Botan::BigInt const& client::prime() const noexcept
    {
        return N_;
    }

    Botan::BigInt const& client::generator() const noexcept
    {
        return g_;
    }

    Botan::BigInt client::session_key(
        Botan::BigInt const& B, std::string const& username, std::string const& password, Botan::BigInt const& salt)
    {
        auto x = generate_x(username, password, salt, compliance_);

        // u = H(A, B)
        auto u = sha1_hash(compliance_, A_, B);

        // k = 3 for legacy SRP-6, k = H(N, g) in SRP-6a
        // Refer to http://srp.stanford.edu/design.html
        auto const k = 3;
        auto S = Botan::power_mod(B - k * Botan::power_mod(g_, x, N_), a_ + u * x, N_);

        if (compliance_ == compliance::Wow)
        {
            auto K = sha_interleave(S);
            std::reverse(std::begin(K), std::end(K));

            return Botan::BigInt::decode(K);
        }
        else
            return sha1_hash(compliance_, S);
    }
}