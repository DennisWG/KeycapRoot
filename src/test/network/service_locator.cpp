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

#include <keycap/root/network/memory_stream.hpp>
#include <keycap/root/network/registered_message.hpp>
#include <keycap/root/network/service_locator.hpp>
#include <keycap/root/utility/crc32.hpp>

#include <rapidcheck/catch.h>

#include <chrono>

namespace net = keycap::root::network;
namespace util = keycap::root::utility;

struct DummyConnection;

struct ServerService : public net::service<DummyConnection>
{
    ServerService()
      : service{net::service_mode::Server}
    {
    }

    virtual SharedHandler make_handler() override
    {
        return std::make_shared<DummyConnection>(*this);
    }

    net::link_status status = net::link_status::Down;
    std::string data;
};

struct DummyConnection : public net::connection<DummyConnection>, public net::message_handler
{
    DummyConnection(net::service_base& service)
      : connection{service}
      , myService{static_cast<ServerService&>(service)}
    {
        router_.configure_inbound(this);
    }

    bool on_data(net::service_base& service, std::vector<uint8_t> const& data) override
    {
        // string received
        if (data.size() < 15)
        {
            auto received = std::string{std::begin(data), std::end(data)};
            myService.data = received;
        }
        // data received
        else
        {
            net::memory_stream stream(data.begin(), data.end());

            auto msg = net::registered_message::decode(stream);
            myService.data = msg.payload.get_string(strlen("Foobar"));

            msg.payload.clear();
            msg.payload.put("Arrived");
            msg.crc = util::crc32(msg.sender, msg.command, msg.payload);

            stream = msg.encode();
            send(std::vector<uint8>{stream.data(), stream.data() + stream.size()});
        }

        return true;
    }

    bool on_link(net::service_base& service, net::link_status status) override
    {
        myService.status = status;
        return true;
    }

  private:
    ServerService& myService;
};

TEST_CASE("servicce_locator")
{
    net::service_locator locator;

    SECTION("A just initialized locator must not have any services")
    {
        REQUIRE(locator.service_count() == 0);
    }

    SECTION("Locating a new service must increase the service count")
    {
        locator.locate(net::service_type{1}, "localhost", 5567);

        REQUIRE(locator.service_count() == 1);
    }

    SECTION("Locating a service twice must not increase the service count")
    {
        locator.locate(net::service_type{1}, "localhost", 5567);
        locator.locate(net::service_type{1}, "localhost", 5567);

        REQUIRE(locator.service_count() == 1);
    }

    SECTION("When locating an available service, we must connect to it")
    {
        std::string const host = "localhost";
        uint16_t const port = 5568;

        ServerService service;
        service.start(host, port);

        locator.locate(net::service_type{1}, host, port);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        REQUIRE(service.status == net::link_status::Up);
    }

    SECTION("Locating multiple services")
    {
        std::string const host = "localhost";
        uint16_t const port_1 = 5569;
        uint16_t const port_2 = 5570;
        net::service_type const type_1{1};
        net::service_type const type_2{2};

        ServerService service_1;
        service_1.start(host, port_1);

        ServerService service_2;
        service_2.start(host, port_2);

        locator.locate(type_1, host, port_1);
        locator.locate(type_2, host, port_2);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        REQUIRE(service_1.status == net::link_status::Up);
        REQUIRE(service_2.status == net::link_status::Up);
        REQUIRE(locator.service_count() == 2);
    }

    SECTION("Sending data to a service_type must send it to the correct service")
    {
        auto send = [](net::service_locator& locator, net::service_type const type, std::string const& data) {
            net::memory_stream stream;
            stream.put(data);
            locator.send_to(type, stream);
        };

        std::string const host = "localhost";
        uint16_t const port_1 = 5569;
        uint16_t const port_2 = 5570;
        net::service_type const type_1{1};
        net::service_type const type_2{2};

        ServerService service_1;
        service_1.start(host, port_1);

        ServerService service_2;
        service_2.start(host, port_2);

        locator.locate(type_1, host, port_1);
        locator.locate(type_2, host, port_2);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        std::string const data_1 = "To 1!";
        std::string const data_2 = "Hello, 2!";
        send(locator, type_1, data_1);
        send(locator, type_2, data_2);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        REQUIRE(service_1.data == data_1);
        REQUIRE(service_2.data == data_2);
    }

    SECTION("Sending registered messages must yield an answer if handled properly")
    {
        auto send = [](net::service_locator& locator, net::service_type const type, std::string const& data,
                       net::service_locator::registered_callback callback) {
            net::memory_stream stream;
            stream.put(data);
            locator.send_registered(type, stream, callback);
        };

        std::string const host = "localhost";
        uint16_t const port = 5571;
        net::service_type const type{1};

        ServerService service;
        service.start(host, port);

        locator.locate(type, host, port);

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        std::string received_data;
        send(locator, type, "Foobar", [&](net::service_type sender, net::memory_stream data) -> bool {
            received_data = data.get_string(strlen("Arrived"));
            return true;
        });

        std::this_thread::sleep_for(std::chrono::milliseconds{10});

        REQUIRE(service.data == "Foobar");
        REQUIRE(received_data == "Arrived");
    }
}