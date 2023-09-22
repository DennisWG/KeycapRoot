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

#include <keycap/root/network/data_router.hpp>
#include <keycap/root/network/message_handler.hpp>
#include <keycap/root/network/service.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <rapidcheck/catch.h>

namespace net = keycap::root::network;

class TestHandler;

struct DummyService : public net::service_base
{
    DummyService()
      : net::service_base{net::service_type{0}}
    {
    }

    void handle_new_connection(boost::asio::ip::tcp::socket socket) override
    {
    }
};

template <typename Service, typename MessageHandler>
struct DummyConnection
{
};

class TestHandler : public net::message_handler
{
  public:
    explicit TestHandler(net::data_router& router)
      : router_{router}
    {
        router_.configure_inbound(this);
    }

    bool on_data(net::data_router const& router, net::service_type service, std::span<uint8_t> data) override
    {
        OnMessageCalled = true;
        return true;
    }

    bool on_link(net::data_router const& router, net::service_type service, net::link_status status) override
    {
        OnLinkCalled = true;
        return true;
    }

    bool OnMessageCalled = false;
    bool OnLinkCalled = false;

  private:
    net::data_router& router_;
};

TEST_CASE("data_router")
{
    DummyService service;
    net::data_router router;
    TestHandler handler{router};
    TestHandler handler2{router};
    std::vector<uint8_t> data;

    SECTION("data_router::route_inbound() must route messages to properly configured MessageHandlers")
    {
        router.route_inbound(service, data);
        REQUIRE(handler.OnMessageCalled);
        REQUIRE(handler2.OnMessageCalled);
    }

    SECTION("data_router::remove_handler() must remove the given handler and no longer route data to it")
    {
        router.remove_handler(&handler);
        router.route_inbound(service, data);
        REQUIRE_FALSE(handler.OnMessageCalled);
        REQUIRE(handler2.OnMessageCalled);
    }

    SECTION("data_router::route_updated_link_status() must route status to all registered MessageHandlers")
    {
        router.route_updated_link_status(service, net::link_status::Up);
        REQUIRE(handler.OnLinkCalled);
        REQUIRE(handler2.OnLinkCalled);
    }

    SECTION("data_router::remove_handler() must remove the given handler and no longer route link updates to it")
    {
        router.remove_handler(&handler);
        router.route_updated_link_status(service, net::link_status::Down);
        REQUIRE_FALSE(handler.OnLinkCalled);
        REQUIRE(handler2.OnLinkCalled);
    }
}
