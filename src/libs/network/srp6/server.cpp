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

#include <keycap/root/network/srp6/server.hpp>
#include <keycap/root/network/srp6/utility.hpp>

#include <botan_all.h>

namespace srp6 = keycap::root::network::srp6;

namespace keycap::root::network::srp6
{
    server::server(
        group_parameter groupParameter, Botan::BigInt const& v, compliance compliance, Botan::BigInt const& b)
      : server(Botan::BigInt{groupParameter.N}, Botan::BigInt{groupParameter.g}, v, b, compliance)
    {
    }

    server::server(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& v, Botan::BigInt const& b,
        compliance compliance)
      : N_{N}
      , g_{g}
      , v_{v}
      , b_{b}
      , compliance_{compliance}
    {
        // k = 3 for legacy SRP-6, k = H(N, g) in SRP-6a
        // Refer to http://srp.stanford.edu/design.html
        auto const k = 3;
        B_ = (k * v_ + Botan::power_mod(g_, b_, N_)) % N_;
    }

    Botan::BigInt const& server::public_ephemeral_value() const noexcept
    {
        return B_;
    }

    Botan::BigInt server::session_key(Botan::BigInt const& A)
    {
        if ((A % N_) == 0 || A <= 0)
            throw std::exception("Ephemeral value of A must not be 0 (mod N) and not be <= 0!");

        A_ = A;

        // u = H(A, B)
        Botan::BigInt u = sha1_hash(compliance_, A, B_);
        //  S = (Av^u) ^ b
        Botan::BigInt S = Botan::power_mod(A * Botan::power_mod(v_, u, N_), b_, N_);

        if (compliance_ == compliance::Wow)
        {
            auto K = sha_interleave(S);
            std::reverse(std::begin(K), std::end(K));

            return Botan::BigInt::decode(K);
        }

        return sha1_hash(compliance_, S);
    }

    Botan::BigInt server::proof(Botan::BigInt const& clientProof, Botan::BigInt const& sessionKey) const
    {
        // M = H(A, M, K)
        return sha1_hash(compliance_, A_, clientProof, sessionKey);
    }

    compliance server::compliance_mode() const noexcept
    {
        return compliance_;
    }

    Botan::BigInt const& server::prime() const noexcept
    {
        return N_;
    }

    Botan::BigInt const& server::generator() const noexcept
    {
        return g_;
    }

    Botan::BigInt const& server::verifier() const noexcept
    {
        return v_;
    }
}
