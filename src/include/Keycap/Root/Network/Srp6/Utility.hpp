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

#include <botan/bigint.h>

#include <array>
#include <string>
#include <vector>

namespace Keycap::Root::Network::Srp6
{
    std::vector<Botan::byte> encode_flip(const Botan::BigInt& val);

    Botan::secure_vector<Botan::byte> encode_1363_flip(const Botan::BigInt& val, std::size_t bytes);

    Botan::BigInt decode_flip(Botan::secure_vector<Botan::byte> val);

    Botan::BigInt GenerateClientProof(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& s, std::string const& I,
        Botan::BigInt const& A, Botan::BigInt const& B, std::vector<uint8_t> const& S,
        Keycap::Root::Network::Srp6::Compliance compliance);
}
