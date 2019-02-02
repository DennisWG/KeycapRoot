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
    void service_locator::locate(
        service_type type, std::string const& host, uint16_t port,
        std::optional<std::pair<boost::asio::io_service&, located_callback>> callback)
    {
        if (auto itr = services_.find(type.get()); itr != services_.end())
            return;

        if (callback)
            located_callbacks_.try_emplace(type.get(), located_callback_container{callback->first, callback->second});

        auto& service = services_.try_emplace(services_.end(), type.get(), type, this)->second;
        service.start(host, port);
    }

    void service_locator::remove_located_callback(service_type type)
    {
        located_callbacks_.erase(type.get());
    }

    void service_locator::send_to(service_type type, memory_stream const& message)
    {
        registered_message msg;
        msg.crc = utility::crc32(uint64{0}, registered_command::Update, message);
        msg.sender = 0;
        msg.command = registered_command::Update;
        msg.payload = message;

        auto stream = msg.encode();
        send_to_(type, stream);
    }

    void service_locator::send_registered(
        service_type type, memory_stream const& message, boost::asio::io_service& io_service,
        registered_callback callback)
    {
        auto counter = registered_callback_counter_++;

        registered_callbacks_.try_emplace(counter, registered_callback_container{type.get(), io_service, callback});

        registered_message msg;
        msg.crc = utility::crc32(counter, registered_command::Request, message);
        msg.sender = counter;
        msg.command = registered_command::Request;
        msg.payload = message;

        auto stream = msg.encode();
        send_to_(type, stream);
    }

    size_t service_locator::service_count() const
    {
        return services_.size();
    }

    bool service_locator::on_data(data_router const& router, service_type service, std::vector<uint8_t> const& data)
    {
        memory_stream stream(data.begin(), data.end());

        if (!registered_message::can_decode(stream))
            return true;

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

        auto[sender, io_service, _] = itr->second;

        io_service.post([ sender = service_type{sender}, payload = msg.payload, callback = itr->second.calback ]() {
            callback(service_type{sender}, payload);
        });

        registered_callbacks_.erase(itr); // TODO: is this valid?

        return true;
    }

    bool service_locator::on_link(data_router const& router, service_type service, link_status status)
    {
        if (status == link_status::Up)
        {
            if (auto itr = located_callbacks_.find(service.get()); itr != located_callbacks_.end())
            {
                auto[io_service, callback] = itr->second;
                io_service.post([ this, callback = itr->second.calback, service ]() {
                    callback(*this, service);
                    //
                });
            }

            return true;
        }

        schedule_.add(std::chrono::seconds(5), [=](auto error) {
            auto itr = services_.find(service.get());
            if (itr == services_.end())
                return;

            auto& serv = itr->second;
            serv.restart();
        });

        return true;
    }

    void service_locator::send_to_(service_type type, memory_stream const& message)
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

    service_locator::connection::connection(service_base& service, service_locator* locator)
      : base{service}
    {
        router_.configure_inbound(locator);
    }

    service_locator::service::service(service_type type, service_locator* locator)
      : base{service_mode::Client, type}
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