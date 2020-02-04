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

#include "../utility/enum.hpp"
#include "service_base.hpp"

#include <boost/asio.hpp>

#include <memory>
#include <string>
#include <thread>

namespace keycap::root::network
{
    // clang-format off
    // The mode under which the Service will operate
    keycap_enum(service_mode, int,
        // The Service will act as a Client; connecting to listening servers
        Client,
        // The Service will act as a Server; listening for incoming clients
        Server,
    );
    // clang-format on

    // Handles network communication with other services
    template <typename Connection>
    class service : public service_base
    {
      protected:
        using SharedHandler = std::shared_ptr<Connection>;

      public:
        service(service_mode mode, service_type type, int threadCount = 1)
          : service_base{type}
          , thread_count_{threadCount}
          , acceptor_{io_context_}
          , mode_{mode}
        {
        }

        ~service()
        {
            stop();

            for (auto& thread : thread_pool_)
            {
                if (thread.joinable())
                    thread.join();
            }
        }

        // Starts listening for network communications on or connects to the given host and port
        void start(std::string const& host, uint16_t port)
        {
            endpoint_ = resolve(host, port);

            if (mode_ == service_mode::Server)
                listen();
            else if (mode_ == service_mode::Client)
                connect();

            running_ = true;
            run_thread_pool();
        }

        void restart()
        {
            if (mode_ == service_mode::Server)
                listen();
            else if (mode_ == service_mode::Client)
                connect();

            running_ = true;
            run_thread_pool();
        }

        // Stops listening for new connections. Any asynchronous accept operations will be cancelled immediately
        void stop()
        {
            running_ = false;
            acceptor_.close();
            io_context_.stop();
        }

        // Will be called when a new connection was established.
        // This gets called before the link status has been routed and
        // before the connection has started listening for data.
        // May return false to disconnect
        virtual bool on_new_connection(SharedHandler handler)
        {
            return true;
        }

      protected:
        virtual SharedHandler make_handler(boost::asio::ip::tcp::socket socket) = 0;

      private:
        void run_thread_pool()
        {
            if (io_context_.stopped())
                io_context_.restart();

            for (int i = 0; i < thread_count_; ++i)
                thread_pool_.emplace_back([=] {
                    try
                    {
                        io_context_.run();
                    }
                    catch (std::exception const&)
                    {
                        io_context_.run();
                    }
                });
        }

        void handle_new_connection(boost::asio::ip::tcp::socket socket) override
        {
            auto handler = make_handler(std::move(socket));
            handler->get_router().configure_outbound(handler);

            if (!on_new_connection(handler))
            {
                handler->get_router().route_updated_link_status(*this, link_status::Failed);
                return;
            }

            handler->get_router().route_updated_link_status(*this, link_status::Up);
            handler->listen();
        }

        int thread_count_ = 1;
        std::vector<std::thread> thread_pool_;
        boost::asio::ip::tcp::acceptor acceptor_;
        service_mode mode_;
    };
}