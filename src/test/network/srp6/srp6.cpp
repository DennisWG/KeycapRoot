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

#include <keycap/root/network/srp6/client.hpp>
#include <keycap/root/network/srp6/server.hpp>
#include <keycap/root/network/srp6/utility.hpp>

#include <rapidcheck/catch.h>

namespace srp = keycap::root::network::srp6;

TEST_CASE("srp6")
{
    auto parameter = srp::get_parameters(srp::group_parameters::_256);
    constexpr auto compliance = srp::compliance::Wow;

    SECTION("Server proof must be correct when given valid data")
    {
        Botan::BigInt b("1690411537445209277817416133899538702421434810938158426182998584737568201187");
        Botan::BigInt A("0x6fcd7fdf9799a025174f5afe2d59e5d47d02f23c64c643400c591e5e961609d8");
        Botan::BigInt M1("0xadfd0d5fb71ed0680bf7c3ca37215ea722d801e1");
        Botan::BigInt M2("700504732863775958263265562208556932136620103480");

        Botan::BigInt salt("68859801173271747564637603477832205291500145941055860779287735582833867616044");

        std::string username = "USER";
        std::string password = "PASSWORD";
        auto verifier = srp::generate_verifier(username, password, parameter, salt, compliance);

        auto server = srp::server(parameter, Botan::BigInt{verifier}, compliance, b);
        auto sessionKey = server.session_key(A);

        auto M1_S = srp::generate_client_proof(
            server.prime(), server.generator(), salt, username, A, server.public_ephemeral_value(), sessionKey,
            compliance);

        auto M2_S = server.proof(M1_S, sessionKey);

        REQUIRE(M1_S == M1);
        REQUIRE(M2_S == M2);
    }

    SECTION("generate_verifier must always yield the same verifier given the same salt")
    {
        std::string username = "USER";
        std::string password = "PASSWD";

        Botan::BigInt salt{"0x8658BD63DC7B110049E5008E6BDC237788CA21DF9BFBE0255C7A82F22E504A39"};
        Botan::BigInt expected_v{"0x026A88DB43CFAE5551FE073F8699BC1F456B114EAB5E034BE7ACD7A4C27B89A8"};

        auto generated_v = srp::generate_verifier(username, password, parameter, salt, compliance);

        REQUIRE(expected_v == generated_v);
    }

    SECTION("Going through the whole sequence with validated data", "Regression Test")
    {
        Botan::BigInt salt("0x5E01729F0605E6511C2F1EAEBB120A732D2E426871A258A10A7767B3B385760C");
        Botan::BigInt A("0x5a42c68bd5d1a5aa89a80794618a0ba9966ce677b578f2dec770605eaacd4361");
        Botan::BigInt b("0x8ADDDADEAFE428BFA47A62E98D29C86410E1FD79484B995FC255C430BAE11658");
        Botan::BigInt verifier{"0x71D75E755E3D89996E10FA642CDFB9783D4A913FCF1A8D34CAB05A6645072A2C"};

        std::string username = "A";

        Botan::BigInt required_session_key{
            "0xB840C0A3E2EA5AEA4E94C4CA4FE3BDD6E8354032DB9CCDE0F5807296984F9BB254D2500551B0A209"};
        Botan::BigInt required_proof{"0x9F28F1519D913403EE6FE422B4122E8533515C64"};

        srp::server serv{parameter, verifier, compliance, b};

        auto session_key = serv.session_key(A);
        auto M = srp::generate_client_proof(
            serv.prime(), serv.generator(), salt, username, A, serv.public_ephemeral_value(), session_key, compliance);

        REQUIRE(session_key == required_session_key);
        REQUIRE(M == required_proof);
    }

    SECTION("Client/Server must work together and return the expected results")
    {
        Botan::BigInt salt("0x93C999AFF408B3A3CF78D1CF36482C09F7FF0714EF2A982C46C3DFF5A9DC0DE6");
        Botan::BigInt verifier{"0x46FF4F26CC2AB0EA82B849044AC68D6CC772C8232086C890C0FBC5DE13BA3111"};

        std::string account_name = "ADMIN";

        srp::client client{parameter, account_name, compliance};
        srp::server serv{parameter, verifier, compliance};

        auto K_c = client.session_key(serv.public_ephemeral_value(), account_name, account_name, salt);
        auto K_s = serv.session_key(client.public_ephemeral_value());

        auto M_c = srp::generate_client_proof(
            serv.prime(), serv.generator(), salt, account_name, client.public_ephemeral_value(),
            serv.public_ephemeral_value(), K_c, compliance);

        auto M_s = srp::generate_client_proof(
            serv.prime(), serv.generator(), salt, account_name, client.public_ephemeral_value(),
            serv.public_ephemeral_value(), K_s, compliance);

        REQUIRE(K_c == K_s);
        REQUIRE(M_c == M_s);
    }
}
