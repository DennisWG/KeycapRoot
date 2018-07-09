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
#include <keycap/root/network/message_handler.hpp>

#include <boost/uuid/random_generator.hpp>

namespace keycap::root::network
{
    message_handler::message_handler() noexcept
      : uuid_{boost::uuids::random_generator{}()}
    {
    }

    message_handler::message_handler(message_handler const&) noexcept
    {
        uuid_ = boost::uuids::random_generator{}();
    }

    message_handler::~message_handler()
    {
    }

    bool message_handler::operator==(message_handler const& rhs) noexcept
    {
        return uuid_ == rhs.uuid_;
    }
}