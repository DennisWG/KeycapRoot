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
#include <keycap/root/network/registered_message.hpp>
#include <keycap/root/network/service_locator.hpp>
#include <keycap/root/utility/crc32.hpp>

namespace keycap::root::network
{
    void service_locator::locate(service_type type, std::string const& host, uint16_t port)
    {
        if (auto itr = services_.find(type.get()); itr != services_.end())
            return;

        auto& service = services_.try_emplace(type.get(), this).first->second;
        service.start(host, port);
    }

    void service_locator::send_to(service_type type, memory_stream const& message)
    {
        auto itr = services_.find(type.get());
        if (itr == services_.end())
        {
            // TODO: place in queue!
            return;
        }

        auto& service = itr->second;

        if (auto conn = service.connection_.lock())
            conn->send(std::vector<uint8_t>(message.data(), message.data() + message.size()));
        else
        {
            // TODO: place in queue!
        }
    }

    void service_locator::send_registered(service_type type, memory_stream const& message, registered_callback callback)
    {
        auto counter = registered_callback_counter_++;
        registered_callbacks_[counter] = std::make_pair(type.get(), callback);

        registered_message msg;
        msg.crc = utility::crc32(counter, uint16{1}, message);
        msg.sender = counter;
        msg.command = 1; // TODO
        msg.payload = message;

        auto stream = msg.encode();
        send_to(type, stream);
    }

    size_t service_locator::service_count() const
    {
        return services_.size();
    }

    bool service_locator::on_data(data_router const& router, std::vector<uint8_t> const& data)
    {
        memory_stream stream(data.begin(), data.end());

        auto msg = registered_message::decode(stream);

        if (!utility::validate_crc32(msg.crc, msg.sender, msg.command, msg.payload))
        {
            // TODO: implement error handling callback
            return false;
        }

        auto itr = registered_callbacks_.find(msg.sender);
        if (itr == registered_callbacks_.end())
        {
            // TODO: Log this?
            return false;
        }

        auto[sender, callback] = itr->second;
        return callback(service_type{sender}, msg.payload);
    }

    bool service_locator::on_link(data_router const& router, link_status status)
    {
        // TODO
        return true;
    }

    service_locator::connection::connection(service_base& service, service_locator* locator)
      : base{service}
    {
        router_.configure_inbound(locator);
    }

    service_locator::service::service(service_locator* locator)
      : base{service_mode::Client}
      , locator_{locator}
    {
    }

    service_locator::service::SharedHandler service_locator::service::make_handler()
    {
        auto conn = std::make_shared<connection>(*this, locator_);
        connection_ = conn;
        return conn;
    }
}