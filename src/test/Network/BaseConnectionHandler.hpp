#include <Keycap/Root/Network/DataRouter.hpp>

#include <boost/asio.hpp>

#include <memory>

#pragma once

namespace net = Keycap::Root::Network;

class BaseConnectionHandler : public std::enable_shared_from_this<BaseConnectionHandler>
{
protected:
    boost::asio::io_service& service_;
    boost::asio::ip::tcp::socket socket_;
    net::DataRouter const& router_;

    std::string payload_;
    std::array<char, 128> buffer_{ 0 };

    void Send(std::string const& msg)
    {
        payload_ = msg;

        boost::asio::async_write(
            socket_, boost::asio::buffer(payload_),
            [self = shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred){});
    }

    void Receive()
    {
        socket_.async_read_some(
            boost::asio::buffer(buffer_), [self = shared_from_this()](const boost::system::error_code& error,
                std::size_t bytesRead) {
            if (error)
                return;

            self->router_.RouteInbound({ std::begin(self->buffer_), std::begin(self->buffer_) + bytesRead });
            self->Receive();
        });
    }

public:
    BaseConnectionHandler(boost::asio::io_service& service, net::DataRouter const& router)
        : service_{ service }
        , socket_{ service }
        , router_{ router }
    {
    }

    boost::asio::ip::tcp::socket& Socket()
    {
        return socket_;
    }
};