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

#include "connection_base.hpp"
#include "message_handler.hpp"

#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace keycap::root::network
{
    class service_base;

    class data_router
    {
      public:
        data_router();

        // Adds a new message_handler for incoming data
        void configure_inbound(message_handler* handler);

        // Adds a new ConnectionHandler for outgoing data
        void configure_outbound(std::weak_ptr<connection_base> handler);

        // Removes the given MessageeHandler
        void remove_handler(message_handler* handler);

        // Routes the updated link_status to all registered message_handlers
        void route_updated_link_status(service_base& service, link_status status) const;

        // Routes the given data from the given Service to all registered message_handlers
        // Will call every registered message_handler, even if one of them fails
        // Returns whether or not all message_handler succeeded.
        bool route_inbound(service_base& service, std::vector<uint8_t> const& data) const;

        // Routes the given data to the given receiver
        void route_outbound(std::vector<uint8_t> const& data) const;

      private:
        std::vector<message_handler*> inbound_handlers_;
        std::vector<std::weak_ptr<connection_base>> outbound_handlers;
    };
}
