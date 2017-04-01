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

#include <boost/uuid/uuid.hpp>

#include <memory>
#include <vector>

namespace Keycap::Root::Network
{
    class DataRouter;

    // This base class is used for handling incoming network transmissions
    class MessageHandler
    {
      public:
        MessageHandler(DataRouter& router);
        virtual ~MessageHandler();

        bool operator==(MessageHandler const& rhs);

        // Will get called whenever we've received data. 
        // Return type is currently not used.
        virtual bool OnData(std::vector<uint8_t> const& data) = 0;

        // Will get called whenever a connection has been established or after it got disconnected.
        // Will be called before the server socket starts listening for data.
        // Return type is currently not used.
        virtual bool OnLink(LinkStatus status) = 0;

      protected:
        boost::uuids::uuid uuid_;

        // Local reference to unregister this handler upon destruction
        DataRouter& router_;
    };
}
