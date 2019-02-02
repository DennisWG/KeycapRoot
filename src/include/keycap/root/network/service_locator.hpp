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

#include "../types.hpp"
#include "../utility/schedule.hpp"
#include "connection.hpp"
#include "message_handler.hpp"
#include "service.hpp"

#include <optional>
#include <unordered_map>

namespace keycap::root::network
{
    class memory_stream;

    // Abstracts away the need to send messages to a specific connection. Instead, messages will be send to a service
    // that may or may not yet be located. The to be located services must be in service_mode::Server.
    class service_locator : public message_handler
    {
        friend class data_router;

      public:
        using located_callback = std::function<void(service_locator& locator, service_type sender)>;

        // Creates a new service of the given type with the given host and port if none for this type exists.
        // Will call the given callback, when the service is located, if provided
        void locate(
            service_type type, std::string const& host, uint16_t port,
            std::optional<std::pair<boost::asio::io_service&, located_callback>> callback = {});

        // Removes the callback when locating the given service_type
        void remove_located_callback(service_type type);

        // Sends the given message to the given service_type. If the service hasn't been located yet (e.g.
        // disconnected), the message will be placed in a queue and will be send once the service is located.
        void send_to(service_type type, memory_stream const& message);

        using registered_callback = std::function<bool(service_type sender, memory_stream data)>;

        // bool (*)(service_type sender, memory_stream data);
        // Sends the given message to the given service_type. Expects an answer back from the service. If the service
        // hasn't been located yet (e.g. disconnected), the message will be placed in a queue and will be send once the
        // server is located.
        void send_registered(
            service_type type, memory_stream const& message, boost::asio::io_service& io_service,
            registered_callback callback);

        // Returns the number of located services
        size_t service_count() const;

      private:
        bool on_data(data_router const& router, service_type service, std::vector<uint8_t> const& data) override;

        bool on_link(data_router const& router, service_type service, link_status status) override;

        void send_to_(service_type type, memory_stream const& message);

        class connection : public keycap::root::network::connection
        {
            using base = keycap::root::network::connection;

          public:
            connection(service_base& service, service_locator* locator);
        };

        class service : public keycap::root::network::service<connection>
        {
            using base = keycap::root::network::service<connection>;

          public:
            service(service_type type, service_locator* locator);

            virtual SharedHandler make_handler() override;

            std::weak_ptr<connection> connection_;

          private:
            service_locator* locator_ = nullptr;
        };

        std::unordered_map<service_type_t, service_locator::service> services_;

        uint64 registered_callback_counter_ = 0;
        struct registered_callback_container
        {
            service_type_t type;
            boost::asio::io_service& io_service;
            registered_callback calback;
        };
        std::unordered_map<uint64, registered_callback_container> registered_callbacks_;

        struct located_callback_container
        {
            boost::asio::io_service& io_service;
            located_callback calback;
        };
        std::unordered_map<service_type_t, located_callback_container> located_callbacks_;

        utility::schedule schedule_;
    };
}