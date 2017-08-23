#include "BaseConnectionHandler.hpp"

#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/MessageHandler.hpp>
#include <Keycap/Root/Network/Service.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = Keycap::Root::Network;

struct ClientHandler : public BaseConnectionHandler
{
    using Base = BaseConnectionHandler;
    ClientHandler(boost::asio::io_service& service, net::DataRouter const& router)
      : Base(service, router)
    {
    }

    void Listen()
    {
        Send("Ping");
        Receive();
    }
};

struct ServerHandler : public BaseConnectionHandler
{
    using Base = BaseConnectionHandler;
    ServerHandler(boost::asio::io_service& service, net::DataRouter const& router)
      : Base(service, router)
    {
    }

    void Listen()
    {
        Receive();
        Send("Pong");
    }
};

struct TestMessageHandler : public net::MessageHandler
{
    TestMessageHandler(net::DataRouter& router)
      : MessageHandler(router)
    {
    }

    bool OnData(std::vector<uint8_t> const& data) override
    {
        data_ = std::string{std::begin(data), std::end(data)};
        return true;
    }

    bool OnLink(net::LinkStatus status) override
    {
        status_ = status;
        return true;
    }

    net::LinkStatus status_ = net::LinkStatus::Down;
    std::string data_;
};

TEST_CASE("Creating and running services", "[Service]")
{
    std::string const host = "localhost";
    uint16_t const port = 4094;

    SECTION("Using the services")
    {
        net::Service<ServerHandler> server{net::ServiceMode::Server};
        TestMessageHandler serverMessageHandler{server.GetRouter()};
        server.GetRouter().ConfigureInbound(&serverMessageHandler);
        server.Start(host, port);

        net::Service<ClientHandler> client{net::ServiceMode::Client};
        TestMessageHandler clientMessageHandler{client.GetRouter()};
        client.GetRouter().ConfigureInbound(&clientMessageHandler);
        client.Start(host, port);

        std::this_thread::sleep_for(std::chrono::seconds{2});

        REQUIRE(serverMessageHandler.data_ == "Ping");
        REQUIRE(clientMessageHandler.data_ == "Pong");
    }
}