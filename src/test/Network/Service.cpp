#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/MessageHandler.hpp>
#include <Keycap/Root/Network/Service.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = Keycap::Root::Network;

class BaseHandler
{
  protected:
    boost::asio::io_service& service_;
    boost::asio::ip::tcp::socket socket_;
    net::DataRouter const& router_;

    boost::system::error_code Send(std::string const& msg)
    {
        boost::system::error_code ec;
        socket_.write_some(boost::asio::buffer(msg), ec);
        return ec;
    }

    boost::system::error_code Receive()
    {
        std::array<char, 128> buffer{0};
        boost::system::error_code ec;
        auto bytesRead = socket_.read_some(boost::asio::buffer(buffer), ec);
        router_.RouteInbound({std::begin(buffer), std::begin(buffer) + bytesRead});
        return ec;
    }

  public:
    BaseHandler(boost::asio::io_service& service, net::DataRouter const& router)
      : service_{service}
      , socket_{service}
      , router_{router}
    {
    }

    boost::asio::ip::tcp::socket& Socket()
    {
        return socket_;
    }
};

struct ClientHandler : public BaseHandler
{
    using Base = BaseHandler;
    ClientHandler(boost::asio::io_service& service, net::DataRouter const& router)
      : Base(service, router)
    {
    }

    void Start()
    {
        Send("Ping");
        Receive();
    }
};

struct ServerHandler : public BaseHandler
{
    using Base = BaseHandler;
    ServerHandler(boost::asio::io_service& service, net::DataRouter const& router)
      : Base(service, router)
    {
    }

    void Start()
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

    SECTION("")
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