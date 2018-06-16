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

#include <Keycap/Root/Network/Srp6/Server.hpp>
#include <Keycap/Root/Network/Srp6/Utility.hpp>

#include <rapidcheck/catch.h>

namespace srp = Keycap::Root::Network::Srp6;

TEST_CASE("Srp6")
{
    SECTION("Server proof must be correct when given valid data")
    {
        auto parameter = srp::GetParameters(srp::GroupParameters::_256);
        constexpr auto compliance = srp::Compliance::Wow;

        Botan::BigInt b("1690411537445209277817416133899538702421434810938158426182998584737568201187");
        Botan::BigInt A("0x6fcd7fdf9799a025174f5afe2d59e5d47d02f23c64c643400c591e5e961609d8");
        Botan::BigInt M1("0xadfd0d5fb71ed0680bf7c3ca37215ea722d801e1");
        Botan::BigInt M2("700504732863775958263265562208556932136620103480");

        Botan::BigInt salt("68859801173271747564637603477832205291500145941055860779287735582833867616044");

        std::string username = "USER";
        std::string password = "PASSWORD";
        auto verifier = srp::GenerateVerifier(username, password, parameter, salt, compliance);

        auto server = srp::Server(parameter, Botan::BigInt{verifier}, compliance, b);
        auto sessionKey = server.SessionKey(A);

        auto M1_S = srp::GenerateClientProof(
            server.Prime(), server.Generator(), salt, username, A, server.PublicEphemeralValue(), sessionKey,
            compliance);

        auto M2_S = server.Proof(M1_S, sessionKey);

        REQUIRE(M1_S == M1);
        REQUIRE(M2_S == M2);
    }

    SECTION("GenerateVerifier must always yield the same verifier given the same salt")
    {
        auto parameter = srp::GetParameters(srp::GroupParameters::_256);
        constexpr auto compliance = srp::Compliance::Wow;

        std::string username = "USER";
        std::string password = "PASSWD";

        Botan::BigInt salt{"0x8658BD63DC7B110049E5008E6BDC237788CA21DF9BFBE0255C7A82F22E504A39"};
        Botan::BigInt expected_v{"0x026A88DB43CFAE5551FE073F8699BC1F456B114EAB5E034BE7ACD7A4C27B89A8"};

        auto generated_v = srp::GenerateVerifier(username, password, parameter, salt, compliance);

        REQUIRE(expected_v == generated_v);
    }
}
