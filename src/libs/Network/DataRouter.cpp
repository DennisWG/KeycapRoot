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

#include <algorithm>
#include <functional>

namespace net = Keycap::Root::Network;

namespace Keycap::Root::Network
{
    void DataRouter::ConfigureInbound(MessageHandler* handler)
    {
        handlers_.push_back(handler);
    }

    void DataRouter::RemoveHandler(MessageHandler* handler)
    {
        handlers_.erase(std::remove_if(handlers_.begin(), handlers_.end(),
                                       [&](MessageHandler* messageHandler) { return *messageHandler == *handler; }),
                        handlers_.end());
    }

    void DataRouter::RouteUpdatedLinkStatus(LinkStatus status)
    {
        for (auto handler : handlers_)
            handler->OnLink(status);
    }

    void DataRouter::RouteInbound(std::vector<uint8_t> const& data)
    {
        for (auto handler : handlers_)
            handler->OnData(data);
    }
}