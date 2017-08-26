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

#include <gsl/span>

#include <cstdint>
#include <type_traits>
#include <vector>

namespace Keycap::Root::Network
{
    class MemoryStream
    {
      public:
        // Puts a T into the stream
        template <typename T>
        void Put(T const& value)
        {
            gsl::span<uint8_t const> data{reinterpret_cast<uint8_t const*>(&value), sizeof(T)};
            buffer_.insert(buffer_.end(), data.begin(), data.end());
        }

        // Puts a single byte into the stream
        void Put(uint8_t value)
        {
            buffer_.push_back(value);
        }

        // Puts an arbitrary span into the stream
        template <typename T>
        void Put(gsl::span<T> data)
        {
            gsl::span<uint8_t const> d{reinterpret_cast<uint8_t const*>(data.data()), data.length_bytes()};
            buffer_.insert(buffer_.end(), d.begin(), d.end());
        }

        // Puts a span of bytes into the stream
        void Put(gsl::span<uint8_t> data)
        {
            buffer_.insert(buffer_.end(), data.begin(), data.end());
        }

        // Gets a T from the stream
        template <typename T>
        T Get()
        {
            if (sizeof(T) + readPosition_ > buffer_.size())
                throw std::exception("Attempted to read past buffer end!");

            T value = *reinterpret_cast<T const*>(buffer_.data() + readPosition_);
            readPosition_ += sizeof(T);

            return value;
        }

        // Peeks for the given T at the given position in the stream
        template <typename T>
        T Peek(size_t where = 0) const
        {
            if (sizeof(T) + where + readPosition_ > buffer_.size())
                throw std::exception("Attempted to read past buffer end!");

            return *reinterpret_cast<T const*>(buffer_.data() + where + readPosition_);
        }

        // Returns the number of available bytes in the stream
        size_t Size() const
        {
            return buffer_.size() - readPosition_;
        }

        // Removes all elements from the buffer up to the stream's current read position
        void Shrink()
        {
            if (readPosition_ == 0)
                return;

            buffer_.erase(buffer_.begin(), buffer_.begin() + readPosition_);
            readPosition_ = 0;
        }

      private:
        size_t readPosition_ = 0;
        std::vector<uint8_t> buffer_;
    };
}