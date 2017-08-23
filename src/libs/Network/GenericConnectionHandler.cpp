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

#include <Keycap/Root/Network/DataRouter.hpp>
#include <Keycap/Root/Network/GenericConnectionHandler.hpp>

#include <iostream>
#include <vector>

namespace Keycap::Root::Network
{
    GenericConnectionHandler::GenericConnectionHandler(boost::asio::io_service& service, DataRouter& router)
      : router_{router}
      , service_{service}
      , socket_{service}
      , writeStrand_{service}
    {
    }

    boost::asio::ip::tcp::socket& GenericConnectionHandler::Socket()
    {
        return socket_;
    }

    void GenericConnectionHandler::Listen()
    {
        router_.ConfigureOutbound(shared_from_this());
        ReadPacket();
    }

    void GenericConnectionHandler::Send(std::vector<std::uint8_t> const& data)
    {
        service_.post(writeStrand_.wrap([ self = shared_from_this(), data ]() {
            bool isWriteInProgress = !self->sendPacketQueue_.empty();
            self->sendPacketQueue_.push_back(std::move(data));

            if (!isWriteInProgress)
                self->SendData();
        }));
    }

    void GenericConnectionHandler::ReadPacket()
    {
        auto buffer = inPacket_.prepare(512);
        socket_.async_read_some(
            buffer, [self = shared_from_this()](boost::system::error_code const& error, size_t bytesRead) {
                self->inPacket_.commit(bytesRead);
                self->ReadPacketDone(error, bytesRead);
            });
    }

    void GenericConnectionHandler::ReadPacketDone(boost::system::error_code const& error, size_t numBytesRead)
    {
        if (error)
        {
            router_.RouteUpdatedLinkStatus(LinkStatus::Down);
            return;
        }

        std::vector<uint8_t> buffer;
        buffer.resize(numBytesRead);

        inPacket_.sgetn(reinterpret_cast<char*>(buffer.data()), numBytesRead);

        router_.RouteInbound(buffer);

        ReadPacket();
    }

    void GenericConnectionHandler::SendData()
    {
        boost::asio::async_write(
            socket_, boost::asio::buffer(sendPacketQueue_.front()),
            [self = shared_from_this()](boost::system::error_code const& error, size_t) { self->SendDataDone(error); });
    }

    void GenericConnectionHandler::SendDataDone(boost::system::error_code const& error)
    {
        if (error)
            return;

        sendPacketQueue_.pop_front();
        if (!sendPacketQueue_.empty())
            SendData();
    }
}