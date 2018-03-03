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

#include <Keycap/Root/Algorithms/Monads.hpp>
#include <Keycap/Root/Network/Srp6/Server.hpp>

#include <botan/auto_rng.h>
#include <botan/numthry.h>
#include <botan/sha160.h>

namespace srp6 = Keycap::Root::Network::Srp6;

// Scrambles A and B with the given length depending on the given Compliance mode.
auto Scramble(Botan::BigInt const& A, Botan::BigInt const& B, size_t length, srp6::Compliance compliance)
{
    Botan::SHA_1 sha1;

    if (compliance == srp6::Compliance::RFC5054)
    {
        sha1.update(Botan::BigInt::encode_1363(A, length));
        sha1.update(Botan::BigInt::encode_1363(B, length));

        return Botan::BigInt::decode(sha1.final());
    }
    else if (compliance == srp6::Compliance::Wow)
    {
        auto encodeFlipUpdate = [length, &sha1](Botan::BigInt const& value) {
            auto result = Botan::BigInt::encode_1363(value, length);
            std::reverse(std::begin(result), std::end(result));
            sha1.update(result);
        };

        encodeFlipUpdate(A);
        encodeFlipUpdate(B);

        return Botan::BigInt::decode(sha1.final());
    }

    throw std::exception("Unknown compliance mode!");
}

auto ShaInterleaved(std::vector<uint8_t>& S)
{
    using namespace Keycap::Root;

    // Refer to https://www.ietf.org/rfc/rfc2945.txt section "3.1.  Interleaved SHA"

    // skip leading zeroes and one more byte if the remaining sequence has an odd number of elements remaining
    auto begin = std::find_if(S.begin(), S.end(), [](auto&& v) { return v != 0; });
    if (is_odd(std::distance(begin, std::end(S))))
        ++begin;

    // clang-format off

    // seperate even elements from odd ones
    auto bound = std::stable_partition(begin, std::end(S), [&begin](auto&& v) {
        return is_even(array_index(v, *begin));
    });

    // clang-format on

    Botan::SHA_1 sha;
    auto G{sha.process(&*begin, std::distance(begin, bound))};
    auto H{sha.process(&*bound, std::distance(bound, std::end(S)))};

    std::vector<uint8_t> result;
    for (int i = 0; i < G.size(); ++i)
    {
        result.push_back(G[i]);
        result.push_back(H[i]);
    }

    return result;
}

namespace Keycap::Root::Network::Srp6
{
    Server::Server(GroupParameter groupParameter, Botan::BigInt const& v, Compliance compliance, std::size_t secretBits)
      : Server(
            Botan::BigInt{groupParameter.value}, Botan::BigInt{groupParameter.generator}, v,
            Botan::BigInt::decode(Botan::AutoSeeded_RNG().random_vec(secretBits)), compliance)
    {
    }

    Server::Server(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& v, Botan::BigInt const& b,
        Compliance compliance)
      : N_{N}
      , g_{g}
      , v_{Botan::power_mod(g_, v_, N_)}
      , b_{b}
      , compliance_{compliance}
    {
        // k = 3 for legacy SRP-6, k = H(N, g) in SRP-6a
        // Refer to http://srp.stanford.edu/design.html
        auto const k = 3;
        B_ = k * v + g % b;
    }

    Botan::BigInt const& Server::PublicEphemeralValue() const
    {
        return B_;
    }

    std::vector<uint8_t> Server::SessionKey(Botan::BigInt const& A)
    {
        if ((A % N_) == 0 || A < 0)
            throw std::exception("Ephemeral value of A must not be 0 (mod N) and not be < 0!");

        A_ = A;

        // u = H(A, B)
        Botan::BigInt u = Scramble(A, B_, N_.bytes(), compliance_);
        //  S = (Av^u) ^ b
        Botan::BigInt S = Botan::power_mod(A * Botan::power_mod(v_, u, N_), b_, N_);

        if (compliance_ == Compliance::Wow)
        {
            auto tmp = Botan::BigInt::encode(S);
            std::reverse(std::begin(tmp), std::end(tmp));
            return ShaInterleaved(tmp);
        }

        return ShaInterleaved(Botan::BigInt::encode(S));
    }
}