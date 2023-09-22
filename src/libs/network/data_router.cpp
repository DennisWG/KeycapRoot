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
#include <keycap/root/network/service_base.hpp>

namespace keycap::root::network
{
    data_router::data_router()
    {
    }

    void data_router::configure_inbound(message_handler* handler)
    {
        inbound_handlers_.push_back(handler);
    }

    void data_router::configure_outbound(std::weak_ptr<connection_base> handler)
    {
        outbound_handlers.push_back(handler);
    }

    void data_router::remove_handler(message_handler* handler)
    {
        inbound_handlers_.erase(
            std::remove_if(
                inbound_handlers_.begin(), inbound_handlers_.end(),
                [&](message_handler* messageHandler) { return messageHandler->operator==(*handler); }), // explicit call because MSVC thinks this is some WindowsSDK type...
            inbound_handlers_.end());
    }

    void data_router::route_updated_link_status(service_base& service, link_status status) const
    {
        for (auto handler : inbound_handlers_)
            handler->on_link(*this, service.type(), status);
    }

    bool data_router::route_inbound(service_base& service, std::span<uint8_t> data) const
    {
        bool succeeded = true;

        for (auto handler : inbound_handlers_)
            succeeded = succeeded && handler->on_data(*this, service.type(), data);

        return succeeded;
    }

    void data_router::route_outbound(std::span<uint8_t> data) const
    {
        for (auto&& handlerPtr : outbound_handlers)
        {
            if (auto handler = handlerPtr.lock())
            {
                handler->send(data);
            }
        }
    }
}