#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/MessageHandler.hpp>
#include <Keycap/Root/Network/Service.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = Keycap::Root::Network;

class BaseHandler : public std::enable_shared_from_this<BaseHandler>
{
  protected:
    boost::asio::io_service& service_;
    boost::asio::ip::tcp::socket socket_;
    net::DataRouter const& router_;

    std::string payload_;
    std::array<char, 128> buffer_{0};

    void Send(std::string const& msg)
    {
        payload_ = msg;

        boost::asio::async_write(
            socket_, boost::asio::buffer(payload_),
            [me = shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred){});
    }

    void Receive()
    {
        socket_.async_read_some(
            boost::asio::buffer(buffer_), [me = shared_from_this()](const boost::system::error_code& error,
                                                                    std::size_t bytesRead) {
                if (error)
                    return;

                me->router_.RouteInbound({std::begin(me->buffer_), std::begin(me->buffer_) + bytesRead});
                me->Receive();
            });
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