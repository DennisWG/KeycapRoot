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

#include "link_status.hpp"

#include <boost/uuid/uuid.hpp>

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace keycap::root::network
{
    class service_base;
    class connection_base;

    template <typename MessageHandler>
    class data_router
    {
      public:
        data_router()
        {
        }

        // Adds a new MessageHandler for incoming data
        void configure_inbound(MessageHandler* handler)
        {
            inbound_handlers_.push_back(handler);
        }

        // Adds a new ConnectionHandler for outgoing data
        void configure_outbound(std::weak_ptr<connection_base> handler)
        {
            outbound_handlers.push_back(handler);
        }

        // Removes the given MessageeHandler
        void remove_handler(MessageHandler* handler)
        {
            inbound_handlers_.erase(
                std::remove_if(
                    inbound_handlers_.begin(), inbound_handlers_.end(),
                    [&](MessageHandler* messageHandler) { return *messageHandler == *handler; }),
                inbound_handlers_.end());
        }

        // Routes the updated link_status to all registered MessageHandlers
        void route_updated_link_status(service_base& service, link_status status) const
        {
            for (auto handler : inbound_handlers_)
                handler->on_link(service, status);
        }

        // Routes the given data from the given Service to all registered MessageHandlers
        // Will call every registered MessageHandler, even if one of them fails
        // Returns whether or not all MessageHandlers succeeded.
        bool route_inbound(service_base& service, std::vector<uint8_t> const& data) const
        {
            bool succeeded = true;

            for (auto handler : inbound_handlers_)
                succeeded = succeeded && handler->on_data(service, data);

            return succeeded;
        }

        // Routes the given data to the given receiver
        void route_outbound(std::vector<uint8_t> const& data) const
        {
            for (auto&& handlerPtr : outbound_handlers)
            {
                if (auto handler = handlerPtr.lock())
                {
                    handler->send(data);
                }
            }
        }

      private:
        std::vector<MessageHandler*> inbound_handlers_;
        std::vector<std::weak_ptr<connection_base>> outbound_handlers;
    };
}
