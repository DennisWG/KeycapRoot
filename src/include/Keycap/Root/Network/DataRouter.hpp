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

#include "LinkStatus.hpp"
//#include "MessageHandler.hpp"

#include <boost/uuid/uuid.hpp>

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace Keycap::Root::Network
{
    class ServiceBase;
    class ConnectionBase;

    template <typename MessageHandler>
    class DataRouter
    {
      public:
        DataRouter()
        {
        }

        // Adds a new MessageHandler for incoming data
        void ConfigureInbound(MessageHandler* handler)
        {
            inboundHandlers_.push_back(handler);
        }

        // Adds a new ConnectionHandler for outgoing data
        void ConfigureOutbound(std::weak_ptr<ConnectionBase> handler)
        {
            outboundHandlers_.push_back(handler);
        }

        // Removes the given MessageeHandler
        void RemoveHandler(MessageHandler* handler)
        {
            inboundHandlers_.erase(
                std::remove_if(
                    inboundHandlers_.begin(), inboundHandlers_.end(),
                    [&](MessageHandler* messageHandler) { return *messageHandler == *handler; }),
                inboundHandlers_.end());
        }

        // Routes the updated LinkStatus to all registered MessageHandlers
        void RouteUpdatedLinkStatus(ServiceBase& service, LinkStatus status) const
        {
            for (auto handler : inboundHandlers_)
                handler->OnLink(service, status);
        }

        // Routes the given data from the given Service to all registered MessageHandlers
        void RouteInbound(ServiceBase& service, std::vector<uint8_t> const& data) const
        {
            for (auto handler : inboundHandlers_)
                handler->OnData(service, data);
        }

        // Routes the given data to the given receiver
        void RouteOutbound(std::vector<uint8_t> const& data) const
        {
            for (auto&& handlerPtr : outboundHandlers_)
            {
                if (auto handler = handlerPtr.lock())
                {
                    handler->Send(data);
                }
            }
        }

      private:
        std::vector<MessageHandler*> inboundHandlers_;
        std::vector<std::weak_ptr<ConnectionBase>> outboundHandlers_;
    };
}
