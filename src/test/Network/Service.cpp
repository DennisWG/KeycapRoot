//#include "BaseConnectionHandler.hpp"

#include <Keycap/Root/Network/Connection.hpp>
#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/MessageHandler.hpp>
#include <Keycap/Root/Network/Service.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = Keycap::Root::Network;

namespace ServiceTest
{
    struct ClientConnection;

    struct ClientService : public net::Service<ClientConnection>
    {
        ClientService()
          : Service{net::ServiceMode::Client}
        {
        }

        net::LinkStatus status = net::LinkStatus::Down;
        std::string data;
    };

    struct ClientConnection : public net::Connection<ClientConnection>, public net::MessageHandler
    {
        ClientConnection(net::ServiceBase& service)
          : Connection{service}
          , myService{static_cast<ClientService&>(service)}
        {
            router_.ConfigureInbound(this);
        }

        void Listen()
        {
            std::string msg{"Ping"};

            Send({msg.begin(), msg.end()});
            Connection::Listen();
        }

        bool OnData(net::ServiceBase& service, std::vector<uint8_t> const& data) override
        {
            auto received = std::string{std::begin(data), std::end(data)};
            myService.data = received;

            return true;
        }

        bool OnLink(net::ServiceBase& service, net::LinkStatus status) override
        {
            myService.status = status;
            return true;
        }

      private:
        ClientService& myService;
    };

    struct DummyConnection;

    struct ServerService : public net::Service<DummyConnection>
    {
        ServerService()
          : Service{net::ServiceMode::Server}
        {
        }

        net::LinkStatus status = net::LinkStatus::Down;
        std::string data;
    };

    struct DummyConnection : public net::Connection<DummyConnection>, public net::MessageHandler
    {
        DummyConnection(net::ServiceBase& service)
          : Connection{service}
          , myService{static_cast<ServerService&>(service)}
        {
            router_.ConfigureInbound(this);
        }

        bool OnData(net::ServiceBase& service, std::vector<uint8_t> const& data) override
        {
            auto received = std::string{std::begin(data), std::end(data)};
            myService.data = received;

            if (received == "Ping")
            {
                std::string msg{"Pong"};
                Send({msg.begin(), msg.end()});
            }

            return true;
        }

        bool OnLink(net::ServiceBase& service, net::LinkStatus status) override
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
            server.Start(host, port);

            ClientService client;
            client.Start(host, port);

            std::this_thread::sleep_for(std::chrono::seconds{2});

            REQUIRE(server.data == "Ping");
            REQUIRE(server.status == net::LinkStatus::Up);

            REQUIRE(client.data == "Pong");
            REQUIRE(client.status == net::LinkStatus::Up);
        }
    }
}