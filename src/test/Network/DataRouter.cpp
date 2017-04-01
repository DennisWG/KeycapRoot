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

#include <rapidcheck/catch.h>

namespace net = Keycap::Root::Network;

class TestHandler final : public net::MessageHandler
{
  public:
    TestHandler(net::DataRouter& router)
      : MessageHandler{router}
    {
        router_.ConfigureInbound(this);
    }

    bool OnMessage(std::vector<uint8_t> const& data) override
    {
        OnMessageCalled = true;
        return true;
    }

    bool OnMessageCalled = false;
};

TEST_CASE("DataRouter")
{
    net::DataRouter router;
    TestHandler handler{router};
    std::vector<uint8_t> data;

    SECTION("DataRouter::RouteInbound() must route messages to properly configured MessageHandlers")
    {
        router.RouteInbound(data);
        REQUIRE(handler.OnMessageCalled);
    }

    SECTION("DataRouter::RemoveHandler() must remove the given handler")
    {
        router.RemoveHandler(&handler);
        router.RouteInbound(data);
        REQUIRE_FALSE(handler.OnMessageCalled);
    }
}
