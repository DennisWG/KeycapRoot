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

#pragma once

#include "../Utility/Enum.hpp"
#include "DataRouter.hpp"

#include <boost/asio.hpp>

#include <memory>
#include <string>
#include <thread>

namespace Keycap::Root::Network
{
    // clang-format off
    // The mode under which the Service will operate
    keycap_enum(ServiceMode,
        // The Service will act as a Client; connecting to listening servers
        Client,
        // The Service will act as a Server; listening for incoming clients
        Server,
    );
    // clang-format on

    // Handles network communication with other services
    template <typename ConnectionHandler>
    class Service
    {
        using SharedHandler = std::shared_ptr<ConnectionHandler>;

      public:
        Service(ServiceMode mode, int threadCount = 1)
          : threadCount_{threadCount}
          , acceptor_{ioService_}
          , mode_{mode}
        {
        }

        ~Service()
        {
            Stop();

            for (auto& thread : threadPool_)
            {
                if (thread.joinable())
                    thread.join();
            }
        }

        // Starts listening for network communications on or connects to the given host and port
        void Start(std::string const& host, uint16_t port)
        {
            if (mode_ == ServiceMode::Server)
                BeginListen(host, port);
            else if (mode_ == ServiceMode::Client)
                ConnectTo(host, port);
        }

        // Stops listening for new connections. Any asynchronous accept operations will be cancelled immediately
        void Stop()
        {
            acceptor_.close();
			ioService_.stop();
        }

        // Returns the DataRouter used by this service to route data
        DataRouter& GetRouter()
        {
            return router_;
        }

      private:
        void BeginListen(std::string const& host, uint16_t port)
        {
            auto handler = std::make_shared<ConnectionHandler>(ioService_, router_);

            boost::asio::ip::tcp::resolver resolver{ioService_};
            auto ep = resolver.resolve({host, ""})->endpoint();
            boost::asio::ip::tcp::endpoint endpoint{ep.address(), port};

            acceptor_.open(endpoint.protocol());
            acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address{true});
            acceptor_.bind(endpoint);
            acceptor_.listen();

            acceptor_.async_accept(handler->Socket(), [=](auto errorCode) { HandleNewConnection(handler, errorCode); });

            for (int i = 0; i < threadCount_; ++i)
                threadPool_.emplace_back([=] { ioService_.run(); });
        }

        void ConnectTo(std::string const& host, uint16_t port)
        {
            boost::asio::ip::tcp::resolver resolver{ioService_};
            auto ep = resolver.resolve({host, ""})->endpoint();
            ep.port(port);

            auto handler = std::make_shared<ConnectionHandler>(ioService_, router_);
            boost::system::error_code error;
            handler->Socket().async_connect(ep, [=](auto errorCode) { HandleNewConnection(handler, errorCode); });

            for (int i = 0; i < threadCount_; ++i)
                threadPool_.emplace_back([=] { ioService_.run(); });
        }

        void HandleNewConnection(SharedHandler handler, boost::system::error_code const& error)
        {
            if (error)
                return;

            router_.RouteUpdatedLinkStatus(LinkStatus::Up);

            handler->Start();

            if (mode_ == ServiceMode::Server)
            {
                auto newHandler = std::make_shared<ConnectionHandler>(ioService_, router_);
                acceptor_.async_accept(newHandler->Socket(),
                                       [=](auto errorCode) { HandleNewConnection(newHandler, errorCode); });
            }
        }

        int threadCount_ = 1;
        std::vector<std::thread> threadPool_;
        boost::asio::io_service ioService_;
        boost::asio::ip::tcp::acceptor acceptor_;
        DataRouter router_;
        ServiceMode mode_;
    };
}