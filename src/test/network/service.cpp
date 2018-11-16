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

#include <keycap/root/network/connection.hpp>
#include <keycap/root/network/data_router.hpp>
#include <keycap/root/network/message_handler.hpp>
#include <keycap/root/network/service.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = keycap::root::network;

namespace ServiceTest
{
    struct ClientConnection;

    struct ClientService : public net::service<ClientConnection>
    {
        ClientService()
          : service{net::service_mode::Client, net::service_type{0}}
        {
        }

        virtual SharedHandler make_handler() override
        {
            return std::make_shared<ClientConnection>(*this);
        }

        net::link_status status = net::link_status::Down;
        std::string data;
    };

    struct ClientConnection : public net::connection, public net::message_handler
    {
        ClientConnection(net::service_base& service)
          : connection{service}
          , myService{static_cast<ClientService&>(service)}
        {
            router_.configure_inbound(this);
        }

        void listen()
        {
            std::string msg{"Ping"};

            send({msg.begin(), msg.end()});
            connection::listen();
        }

        bool on_data(net::data_router const& router, net::service_type service, std::vector<uint8_t> const& data) override
        {
            auto received = std::string{std::begin(data), std::end(data)};
            myService.data = received;

            return true;
        }

        bool on_link(net::data_router const& router, net::service_type service, net::link_status status) override
        {
            myService.status = status;
            return true;
        }

      private:
        ClientService& myService;
    };

    struct DummyConnection;

    struct ServerService : public net::service<DummyConnection>
    {
        ServerService()
          : service{net::service_mode::Server, net::service_type{0}}
        {
        }

        virtual SharedHandler make_handler() override
        {
            return std::make_shared<DummyConnection>(*this);
        }

        net::link_status status = net::link_status::Down;
        std::string data;
    };

    struct DummyConnection : public net::connection, public net::message_handler
    {
        DummyConnection(net::service_base& service)
          : connection{service}
          , myService{static_cast<ServerService&>(service)}
        {
            router_.configure_inbound(this);
        }

        bool on_data(net::data_router const& router, net::service_type service, std::vector<uint8_t> const& data) override
        {
            auto received = std::string{std::begin(data), std::end(data)};
            myService.data = received;

            if (received == "Ping")
            {
                std::string msg{"Pong"};
                send({msg.begin(), msg.end()});
            }

            return true;
        }

        bool on_link(net::data_router const& router, net::service_type service, net::link_status status) override
        {
            myService.status = status;
            return true;
        }

      private:
        ServerService& myService;
    };

    TEST_CASE("Creating and running services", "[Service]")
    {
        std::string const host = "localhost";
        uint16_t const port = 4094;

        SECTION("Using the services")
        {
            ServerService server;
            server.start(host, port);

            ClientService client;
            client.start(host, port);

            std::this_thread::sleep_for(std::chrono::milliseconds{10});

            REQUIRE(server.data == "Ping");
            REQUIRE(server.status == net::link_status::Up);

            REQUIRE(client.data == "Pong");
            REQUIRE(client.status == net::link_status::Up);
        }
    }
}