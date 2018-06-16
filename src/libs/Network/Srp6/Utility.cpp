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

#include <Keycap/Root/Network/Srp6/GroupParameters.hpp>
#include <Keycap/Root/Network/Srp6/Utility.hpp>
#include <Keycap/Root/Utility/String.hpp>

#include <botan/numthry.h>
#include <botan/sha160.h>

namespace Keycap::Root::Network::Srp6
{
    std::vector<Botan::byte> encode_flip(const Botan::BigInt& val)
    {
        std::vector<Botan::byte> res(Botan::BigInt::encode(val));
        std::reverse(res.begin(), res.end());
        return res;
    }

    Botan::secure_vector<Botan::byte> encode_1363_flip(const Botan::BigInt& val, std::size_t bytes)
    {
        Botan::secure_vector<Botan::byte> res(Botan::BigInt::encode_1363(val, bytes));
        std::reverse(res.begin(), res.end());
        return res;
    }

    Botan::BigInt decode_flip(Botan::secure_vector<Botan::byte> val)
    {
        std::reverse(val.begin(), val.end());
        return Botan::BigInt::decode(val);
    }

    auto GenerateX(
        std::string const& username, std::string const& password, Botan::BigInt const& salt, Compliance compliance)
    {
        Botan::SHA_1 sha;
        sha.update(username);
        sha.update(":");
        sha.update(password);
        auto hash = sha.final();

        if (compliance == Compliance::RFC5054)
            sha.update(Botan::BigInt::encode(salt));
        else if (compliance == Compliance::Wow)
            sha.update(encode_flip(salt));

        sha.update(hash);

        if (compliance == Compliance::RFC5054)
            return Botan::BigInt::decode(sha.final());
        else if (compliance == Compliance::Wow)
            return decode_flip(sha.final());

        throw std::exception("Unknown compliance mode!");
    }

    Botan::BigInt GenerateVerifier(
        std::string username, std::string password, GroupParameter groupParameter, Botan::BigInt const& salt,
        Compliance compliance)
    {
        username = Utility::ToUpper(username);
        password = Utility::ToUpper(password);

        auto x = GenerateX(username, password, salt, compliance);

        Botan::BigInt N{groupParameter.N};
        Botan::BigInt g{groupParameter.g};
        Botan::BigInt v = Botan::power_mod(g, x, N);

        return v;
    }

    template<>
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

    template <>
    std::array<uint8_t, 20> ToArray(Botan::BigInt const& value, Compliance compliance)
    {
        if (compliance == Compliance::RFC5054)
        {
            std::array<uint8_t, 20> array;
            auto tmp = Botan::BigInt::encode_1363(value, 20);
            std::copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }
        else if (compliance == Compliance::Wow)
        {
            std::array<uint8_t, 20> array;
            auto tmp = Botan::BigInt::encode_1363(value, 20);
            std::reverse_copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }

        throw std::exception("Unknown compliance mode!");
    }

    Botan::BigInt GenerateClientProof(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& salt, std::string const& I,
        Botan::BigInt const& A, Botan::BigInt const& B, std::vector<uint8_t> const& sessionKey,
        Keycap::Root::Network::Srp6::Compliance compliance)
    {
        Botan::SHA_1 sha1;

        auto nHash{sha1.process(encode_flip(N))};
        auto gHash{sha1.process(encode_flip(g))};
        auto iHash{sha1.process(I)};

        for (size_t i = 0; i < nHash.size(); ++i)
            nHash[i] ^= gHash[i];

        sha1.update(nHash);
        sha1.update(iHash);
        sha1.update(encode_flip(salt));
        sha1.update(encode_flip(A));
        sha1.update(encode_flip(B));
        sha1.update(sessionKey);
        return decode_flip(sha1.final());
    }
}
