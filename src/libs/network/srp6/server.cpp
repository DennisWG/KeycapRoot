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

#include <keycap/root/algorithms/monads.hpp>
#include <keycap/root/network/srp6/server.hpp>
#include <keycap/root/network/srp6/utility.hpp>

#include <botan/numthry.h>
#include <botan/sha160.h>

namespace srp6 = keycap::root::network::srp6;

// Scrambles A and B with the given length depending on the given Compliance mode.
auto Scramble(Botan::BigInt const& A, Botan::BigInt const& B, size_t length, srp6::compliance compliance)
{
    Botan::SHA_1 sha1;

    if (compliance == srp6::compliance::RFC5054)
    {
        sha1.update(Botan::BigInt::encode_1363(A, length));
        sha1.update(Botan::BigInt::encode_1363(B, length));

        return Botan::BigInt::decode(sha1.final());
    }
    else if (compliance == srp6::compliance::Wow)
    {
        sha1.update(srp6::encode_1363_flip(A, length));
        sha1.update(srp6::encode_1363_flip(B, length));

        return srp6::decode_flip(sha1.final());
    }

    throw std::exception("Unknown compliance mode!");
}

/* Refer to https://www.ietf.org/rfc/rfc2945.txt section "3.1.  Interleaved SHA" */
auto ShaInterleaved(std::vector<uint8_t> S)
{
    using namespace keycap::root;

    // To compute this function, remove all leading zero bytes from the input.
    auto begin = std::find_if(S.begin(), S.end(), [](auto&& v) { return v != 0; });

    // If the length of the resulting string is odd, also remove the first byte.
    if (is_odd(std::distance(begin, std::end(S))))
        ++begin;

    // clang-format off

    // Extract the even-numbered bytes into a string E and the odd-numbered bytes into a string F
    auto bound = std::stable_partition(begin, std::end(S), [&begin](auto&& v) {
        return is_even(array_index(v, *begin));
    });

    // clang-format on

    // Hash each one with regular SHA1
    Botan::SHA_1 sha;
    auto G{sha.process(&*begin, std::distance(begin, bound))};
    auto H{sha.process(&*bound, std::distance(bound, std::end(S)))};

    // Interleave the two hashes back together to form the output
    std::vector<uint8_t> result;
    result.reserve(2 * G.size());
    for (int i = 0; i < G.size(); ++i)
    {
        result.push_back(G[i]);
        result.push_back(H[i]);
    }

    return result;
}

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
        Botan::BigInt u = Scramble(A, B_, N_.bytes(), compliance_);
        //  S = (Av^u) ^ b
        Botan::BigInt S = Botan::power_mod(A * Botan::power_mod(v_, u, N_), b_, N_);

        if (compliance_ == compliance::Wow)
        {
            auto K = ShaInterleaved(encode_flip(S));
            std::reverse(std::begin(K), std::end(K));

            return Botan::BigInt::decode(K);
        }

        return S;
    }

    Botan::BigInt server::proof(Botan::BigInt const& clientProof, Botan::BigInt const& sessionKey) const
    {
        // M = H(A, M, K)
        Botan::SHA_1 sha1;

        sha1.update(encode_flip(A_));
        sha1.update(encode_flip(clientProof));
        sha1.update(encode_flip(sessionKey));

        return decode_flip(sha1.final());
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
