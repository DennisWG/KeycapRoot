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

#include "../Utility/Utility.hpp"
#include "ConnectionBase.hpp"
#include "DataRouter.hpp"
#include "ServiceBase.hpp"

namespace Keycap::Root::Network
{
    class ServiceBase;

    template <typename MessageHandler>
    class Connection : public ConnectionBase
    {
      public:
        Connection(ServiceBase& service)
          : ConnectionBase{service.IoService()}
          , service_{service}
        {
        }

        // Returns the socket used by the connection handler
        boost::asio::ip::tcp::socket& Socket()
        {
            return socket_;
        }

        // The connection handler will start to asynchronously listen for incoming data
        void Listen()
        {
            ReadPacket();
        }

        // Sends the given data asynchronously
        void Send(std::vector<std::uint8_t> const& data)
        {
            ioService_.post(writeStrand_.wrap([ self = Utility::shared_from_that(this), data ]() {
                bool isWriteInProgress = !self->sendPacketQueue_.empty();
                self->sendPacketQueue_.push_back(std::move(data));

                if (!isWriteInProgress)
                    self->SendData();
            }));
        }

        DataRouter<MessageHandler>& GetRouter()
        {
            return router_;
        }

        // Gets called whenever data has been received
        // virtual bool ReadDone(std::vector<std::uint8_t> const& data) = 0;

      private:
        void ReadPacket()
        {
            auto buffer = inPacket_.prepare(512);
            socket_.async_read_some(
                buffer, [self
                         = Utility::shared_from_that(this)](boost::system::error_code const& error, size_t bytesRead) {
                    self->inPacket_.commit(bytesRead);
                    self->ReadPacketDone(error, bytesRead);
                });
        }

        void ReadPacketDone(boost::system::error_code const& error, size_t numBytesRead)
        {
            if (error)
            {
                router_.RouteUpdatedLinkStatus(service_, LinkStatus::Down);
                return;
            }

            std::vector<uint8_t> buffer;
            buffer.resize(numBytesRead);

            inPacket_.sgetn(reinterpret_cast<char*>(buffer.data()), numBytesRead);

            router_.RouteInbound(service_, buffer);

            ReadPacket();
        }

        void SendData() override
        {
            boost::asio::async_write(socket_, boost::asio::buffer(sendPacketQueue_.front()), [
                self = Utility::shared_from_that(this), fn = &Connection::SendDataDone
            ](boost::system::error_code const& error, size_t) { self->SendDataDone(error); });
        }

        void SendDataDone(boost::system::error_code const& error) override
        {
            if (error)
                return;

            sendPacketQueue_.pop_front();
            if (!sendPacketQueue_.empty())
                SendData();
        }

      protected:
        DataRouter<MessageHandler> router_;
        ServiceBase& service_;
    };
}