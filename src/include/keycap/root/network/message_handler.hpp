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
#include "service_type.hpp"

#include <boost/uuid/uuid.hpp>

#include <memory>
#include <span>
#include <vector>

namespace keycap::root::network
{
    class data_router;

    class service_base;

    // This base class is used for handling incoming network transmissions
    class message_handler
    {
      public:
        message_handler() noexcept;
        message_handler(message_handler const& rhs) noexcept;
        virtual ~message_handler();

        bool operator==(message_handler const& rhs) noexcept;

        // Will get called whenever we've received data.
        virtual bool on_data(data_router const& router, service_type service, std::span<uint8_t> data) = 0;

        // Will get called whenever a connection has been established or after it got disconnected.
        // Will be called before the server socket starts listening for data.
        virtual bool on_link(data_router const& router, service_type service, link_status status) = 0;

      protected:
        boost::uuids::uuid uuid_;
    };
}
