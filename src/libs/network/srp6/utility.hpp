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

#include <keycap/root/algorithms/monads.hpp>
#include <keycap/root/network/srp6/compliance.hpp>
#include <keycap/root/network/srp6/utility.hpp>

#include <botan_all.h>

namespace srp6 = keycap::root::network::srp6;


/* Refer to https://www.ietf.org/rfc/rfc2945.txt section "3.1.  Interleaved SHA" */
inline auto sha_interleave(Botan::BigInt const& S)
{
    using namespace keycap::root;

    auto s = srp6::encode_flip(S);

    // To compute this function, remove all leading zero bytes from the input.
    auto begin = std::find_if(s.begin(), s.end(), [](auto&& v) { return v != 0; });

    // If the length of the resulting string is odd, also remove the first byte.
    if (is_odd(std::distance(begin, std::end(s))))
        ++begin;

    // clang-format off

    // Extract the even-numbered bytes into a string E and the odd-numbered bytes into a string F
    auto bound = std::stable_partition(begin, std::end(s), [&begin](auto&& v) {
        return is_even(array_index(v, *begin));
    });

    // clang-format on

    // Hash each one with regular SHA1
    auto sha = Botan::HashFunction::create("SHA-1");
    auto G{sha->process(&*begin, std::distance(begin, bound))};
    auto H{sha->process(&*bound, std::distance(bound, std::end(s)))};

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

template <typename T>
void hash_one(Botan::HashFunction& sha, T&& t, srp6::compliance compliance)
{
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_same_v<U, Botan::BigInt>)
    {
        if (compliance == srp6::compliance::Wow)
            sha.update(srp6::encode_flip(std::forward<T>(t)));
        else
            sha.update(Botan::BigInt::encode(t));
    }
    else if constexpr (std::is_same_v<U, Botan::secure_vector<uint8_t>>)
        sha.update(t);
    else if constexpr (std::is_same_v<U, std::vector<uint8_t>>)
        sha.update(t);
    else if constexpr (std::is_same_v<U, std::string>)
        sha.update(t);
    else
        sha.update(std::forward<T>(t), sizeof(T));
}

template <typename... ARGS>
auto sha1_hash(srp6::compliance compliance, ARGS&&... args)
{
    auto sha = Botan::HashFunction::create("SHA-1");

    (hash_one(*sha, args, compliance), ...);

    if (compliance == srp6::compliance::Wow)
        return srp6::decode_flip(sha->final());
    else
        return Botan::BigInt::decode(sha->final());
}

inline auto generate_x(
    std::string const& username, std::string const& password, Botan::BigInt const& salt, srp6::compliance compliance)
{
    auto hash = sha1_hash(compliance, username, std::string{":"}, password);
    return sha1_hash(compliance, salt, hash);
}