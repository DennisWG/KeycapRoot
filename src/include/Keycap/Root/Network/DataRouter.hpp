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

#include <functional>
#include <map>
#include <vector>

namespace Keycap::Root::Network
{
    class MessageHandler;

    class DataRouter
    {
      public:
        // Adds a new MessageHandler for incoming data
        void ConfigureInbound(MessageHandler* handler);

        // Removes the given MessageeHandler
        void RemoveHandler(MessageHandler* handler);

        // Routes the updated LinkStatus to all registered MessageHandlers
        void RouteUpdatedLinkStatus(LinkStatus status);

        // Routes the given data to all registered MessageHandlers
        void RouteInbound(std::vector<uint8_t> const& data);

      private:
        std::vector<MessageHandler*> handlers_;
    };
}
