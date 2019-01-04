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

#include <keycap/root/network/srp6/group_parameters.hpp>
#include <keycap/root/network/srp6/utility.hpp>
#include <keycap/root/utility/string.hpp>

#include <botan/numthry.h>
#include <botan/sha160.h>

namespace keycap::root::network::srp6
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

    Botan::BigInt generate_verifier(
        std::string username, std::string password, group_parameter groupParameter, Botan::BigInt const& salt,
        compliance compliance)
    {
        username = utility::to_upper(username);
        password = utility::to_upper(password);

        auto x = generate_x(username, password, salt, compliance);

        Botan::BigInt N{groupParameter.N};
        Botan::BigInt g{groupParameter.g};
        Botan::BigInt v = Botan::power_mod(g, x, N);

        return v;
    }

    template <>
    std::array<uint8_t, 32> to_array(Botan::BigInt const& value, compliance compliance)
    {
        if (compliance == compliance::RFC5054)
        {
            std::array<uint8_t, 32> array;
            auto tmp = Botan::BigInt::encode_1363(value, 32);
            std::copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }
        else if (compliance == compliance::Wow)
        {
            std::array<uint8_t, 32> array;
            auto tmp = Botan::BigInt::encode_1363(value, 32);
            std::reverse_copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }

        throw std::exception("Unknown compliance mode!");
    }

    template <>
    std::array<uint8_t, 20> to_array(Botan::BigInt const& value, compliance compliance)
    {
        if (compliance == compliance::RFC5054)
        {
            std::array<uint8_t, 20> array;
            auto tmp = Botan::BigInt::encode_1363(value, 20);
            std::copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }
        else if (compliance == compliance::Wow)
        {
            std::array<uint8_t, 20> array;
            auto tmp = Botan::BigInt::encode_1363(value, 20);
            std::reverse_copy(std::begin(tmp), std::end(tmp), std::begin(array));
            return array;
        }

        throw std::exception("Unknown compliance mode!");
    }

    Botan::BigInt generate_client_proof(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& salt, std::string const& I,
        Botan::BigInt const& A, Botan::BigInt const& B, Botan::BigInt const& sessionKey,
        keycap::root::network::srp6::compliance compliance)
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
        sha1.update(encode_flip(sessionKey));
        return decode_flip(sha1.final());
    }
}
