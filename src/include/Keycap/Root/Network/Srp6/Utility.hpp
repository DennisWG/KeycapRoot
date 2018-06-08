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
    // Encodes the given BigInt with the given compliance mode and returns it as a vector of bytes
    std::vector<uint8_t> Encode(Botan::BigInt const& value, Compliance compliance);

    // Decodes the given vector of bytes using the given compliance mode into a BigInt
    Botan::BigInt Decode(Botan::secure_vector<uint8_t> vec, Compliance compliance);

    // Encodes the given BigInt to a byte array according to IEEE 1363 in the given compliance mode
    std::array<uint8_t, 32> ToArray(Botan::BigInt const& value, Compliance compliance);

    Botan::BigInt GenerateClientProof(
        Botan::BigInt const& N, Botan::BigInt const& g, Botan::BigInt const& s, std::string const& I,
        Botan::BigInt const& A, Botan::BigInt const& B, std::vector<uint8_t> const& S,
        Keycap::Root::Network::Srp6::Compliance compliance);
}
