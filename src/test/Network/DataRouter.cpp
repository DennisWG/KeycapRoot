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

#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/MessageHandler.hpp>
#include <Keycap/Root/Network/Service.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

#include <rapidcheck/catch.h>

namespace net = Keycap::Root::Network;

class TestHandler;

struct DummyService : public net::ServiceBase
{
    DummyService()
    {
    }

    boost::asio::io_service& IoService() override
    {
        return ioService_;
    }

    boost::asio::io_service ioService_;
};

template <typename Service, typename MessageHandler>
struct DummyConnection
{
};

class TestHandler : public net::MessageHandler
{
  public:
    explicit TestHandler(net::DataRouter<TestHandler>& router)
      : router_{router}
    {
        router_.ConfigureInbound(this);
    }

    bool OnData(net::ServiceBase& service, std::vector<uint8_t> const& data) override
    {
        OnMessageCalled = true;
        return true;
    }

    bool OnLink(net::ServiceBase& service, net::LinkStatus status) override
    {
        OnLinkCalled = true;
        return true;
    }

    bool OnMessageCalled = false;
    bool OnLinkCalled = false;

  private:
    net::DataRouter<TestHandler>& router_;
};

TEST_CASE("DataRouter")
{
    DummyService service;
    net::DataRouter<TestHandler> router;
    TestHandler handler{router};
    TestHandler handler2{router};
    std::vector<uint8_t> data;

    SECTION("DataRouter::RouteInbound() must route messages to properly configured MessageHandlers")
    {
        router.RouteInbound(service, data);
        REQUIRE(handler.OnMessageCalled);
        REQUIRE(handler2.OnMessageCalled);
    }

    SECTION("DataRouter::RemoveHandler() must remove the given handler and no longer route data to it")
    {
        router.RemoveHandler(&handler);
        router.RouteInbound(service, data);
        REQUIRE_FALSE(handler.OnMessageCalled);
        REQUIRE(handler2.OnMessageCalled);
    }

    SECTION("DataRouter::RouteUpdatedLinkStatus() must route status to all registered MessageHandlers")
    {
        router.RouteUpdatedLinkStatus(service, net::LinkStatus::Up);
        REQUIRE(handler.OnLinkCalled);
        REQUIRE(handler2.OnLinkCalled);
    }

    SECTION("DataRouter::RemoveHandler() must remove the given handler and no longer route link updates to it")
    {
        router.RemoveHandler(&handler);
        router.RouteUpdatedLinkStatus(service, net::LinkStatus::Down);
        REQUIRE_FALSE(handler.OnLinkCalled);
        REQUIRE(handler2.OnLinkCalled);
    }
}
