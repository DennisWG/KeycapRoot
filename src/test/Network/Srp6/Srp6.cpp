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
    SECTION("Fill me in")
    {
        auto parameter = srp::GetParameters(srp::GroupParameters::_256);
        constexpr auto compliance = srp::Compliance::Wow;

        Botan::BigInt b("18593985542940560649451045851874319089347482848983190581196134045699448046190");
        Botan::BigInt A("59852229564408135463856204462249479723343699701058170755060257585995770179058");
        Botan::BigInt M1("1198251478626595859038225880380336340559256984824");
        Botan::BigInt M2("859932068100996518188190846072995264590638975226");

        Botan::BigInt salt("0xF4C7DBCA7138DA48D9B7BE55C0C76B1145AF67340CF7A6718D452A563E12A19C");
        Botan::BigInt verifier("0x37A75AE5BCF38899C75D28688C78434CB690657B5D8D77463668B83D0062A186");
        std::string username = "CHAOSVEX";

        auto server = srp::Server(parameter, Botan::BigInt{verifier}, compliance, b);
        auto sessionKey = server.SessionKey(A);

        auto M1_S = srp::GenerateClientProof(
            server.Prime(), server.Generator(), salt, username, A, server.PublicEphemeralValue(), sessionKey,
            compliance);

        auto M2_S = server.Proof(M1_S, sessionKey);

        REQUIRE(M1_S == M1);
        REQUIRE(M2_S == M2);

        REQUIRE(true);
    }
}
