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

#include <Keycap/Root/Network/Srp6/Utility.hpp>

#include <botan/sha160.h>

namespace Keycap::Root::Network::Srp6
{
    std::vector<uint8_t> Encode(Botan::BigInt const& bn, Compliance compliance)
    {
        if (compliance == Compliance::RFC5054)
            return Botan::BigInt::encode(bn);
        else if (compliance == Compliance::Wow)
        {
            std::vector<uint8_t> buffer{Botan::BigInt::encode(bn)};
            std::reverse(std::begin(buffer), std::end(buffer));
            return buffer;
        }

        throw std::exception("Unknown compliance mode!");
    }

    Botan::BigInt Decode(Botan::secure_vector<uint8_t> vec, Compliance compliance)
    {
        if (compliance == Compliance::RFC5054)
            return Botan::BigInt::decode(vec);
        else if (compliance == Compliance::Wow)
        {
            std::reverse(std::begin(vec), std::end(vec));
            return Botan::BigInt::decode(vec);
        }

        throw std::exception("Unknown compliance mode!");
    }

    std::array<uint8_t, 32> ToArray(Botan::BigInt const& value, Compliance compliance)
    {
        if (compliance == Compliance::RFC5054)
        {
            std::array<uint8_t, 32> array;
            auto tmp = Botan::BigInt::encode_1363(value, 32);
            std::copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }
        else if (compliance == Compliance::Wow)
        {
            std::array<uint8_t, 32> array;
            auto tmp = Botan::BigInt::encode_1363(value, 32);
            std::reverse_copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }

        throw std::exception("Unknown compliance mode!");
    }

    Botan::BigInt GenerateClientProof(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& s, std::string const& I,
        Botan::BigInt const& A, Botan::BigInt const& B, std::vector<uint8_t> const& S,
        Keycap::Root::Network::Srp6::Compliance compliance)
    {
        Botan::SHA_1 sha1;

        auto nHash{sha1.process(Encode(N, compliance))};
        auto gHash{sha1.process(Encode(g, compliance))};
        auto iHash{sha1.process(I)};

        for (size_t i = 0; i < nHash.size(); ++i)
            nHash[i] ^= gHash[i];

        sha1.update(nHash);
        sha1.update(iHash);
        sha1.update(Encode(s, compliance));
        sha1.update(Encode(A, compliance));
        sha1.update(Encode(B, compliance));
        sha1.update(S);
        return Decode(sha1.final(), compliance);
    }
} // namespace Keycap::Root::Network::Srp6
