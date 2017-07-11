#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/Service.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = Keycap::Root::Network;

class TestHandler
{
    boost::asio::io_service& service_;
    boost::asio::ip::tcp::socket socket_;

  public:
    TestHandler(boost::asio::io_service& service, net::DataRouter const& router)
      : service_{service}
      , socket_{service}
    {
    }

    void Start()
    {
        std::string msg{"Connected"};
        socket_.write_some(boost::asio::buffer(msg));
    }

    boost::asio::ip::tcp::socket& Socket()
    {
        return socket_;
    }
};

class TestSocket
{
    boost::asio::ip::tcp::socket socket_;
    boost::asio::io_service& ioService_;

    size_t const static bufferSize = 128;

  public:
    TestSocket(boost::asio::io_service& ioService)
      : socket_{ioService}
      , ioService_{ioService}
    {
        Received.resize(bufferSize);
    }

    boost::system::error_code ConnectTo(std::string const& host, uint16_t port)
    {
        boost::asio::ip::tcp::resolver resolver{ioService_};
        auto ep = resolver.resolve({host, ""})->endpoint();

        boost::asio::ip::tcp::endpoint endpoint{ep.address(), port};

        boost::system::error_code error;
        socket_.connect({ep.address(), port}, error);

        if (error)
            return error;

        std::array<char, bufferSize> buffer{0};
        auto bytesRead = socket_.read_some(boost::asio::buffer(buffer), error);

        Received = std::string{std::begin(buffer), std::begin(buffer) + bytesRead};

        return error;
    }

    std::string Received;
};

TEST_CASE("Creating and running services in server mode", "[Service]")
{
    std::string const host = "localhost";
    uint16_t const port = 4094;

    SECTION("Accepting a new connection must call Start in the ConnectionHandler")
    {
        net::Service<TestHandler> service{net::ServiceMode::Server};
        service.Start(host, port);

        boost::asio::io_service ioService;
        TestSocket clientSocket{ioService};

        REQUIRE_FALSE(clientSocket.ConnectTo(host, port));
        REQUIRE(clientSocket.Received == "Connected");
    }

    SECTION("Services also accept IPs as a parameter in ::Start()")
    {
        std::string const host = "127.0.0.1";

        net::Service<TestHandler> service{net::ServiceMode::Server};
        service.Start(host, port);

        boost::asio::io_service ioService;
        TestSocket clientSocket{ioService};

        REQUIRE_FALSE(clientSocket.ConnectTo(host, port));
        REQUIRE(clientSocket.Received == "Connected");
    }
}