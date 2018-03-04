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
            if constexpr(has_encode_method<T>::value)
            {
                return T::Encode(*this);
            }
            gsl::span<uint8_t const> data{reinterpret_cast<uint8_t const*>(&value), sizeof(T)};
            buffer_.insert(buffer_.end(), data.begin(), data.end());
        }

        template <typename T, size_t NumElements>
        void Put(std::array<T, NumElements> value)
        {
            gsl::span<uint8_t const> data{reinterpret_cast<uint8_t const*>(value.data()),
                                          reinterpret_cast<uint8_t const*>(value.data() + NumElements)};

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

        // Puts a std::string into the stream
        void Put(std::string const& string)
        {
            gsl::span<uint8_t const> d{reinterpret_cast<uint8_t const*>(string.data()),
                                       reinterpret_cast<uint8_t const*>(string.data() + string.size())};
            buffer_.insert(buffer_.end(), d.begin(), d.end());
        }

        // Gets a T from the stream
        template <typename T>
        T Get()
        {
            if constexpr(has_decode_method<T>::value)
            {
                return T::Decode(*this);
            }
            else
            {
                if (sizeof(T) + readPosition_ > buffer_.size())
                    throw std::exception("Attempted to read past buffer end!");

                T value = *reinterpret_cast<T const*>(buffer_.data() + readPosition_);
                readPosition_ += sizeof(T);

                return value;
            }
        }

        // Gets an array from the stream
        template <typename T, size_t NumElements>
        std::array<T, NumElements> Get()
        {
            std::array<T, NumElements> array;
            for (int i = 0; i < NumElements; ++i)
                array[i] = Get<T>();

            return array;
        }

        // Returns a std::string with the given size from the stream
        std::string GetString(size_t size)
        {
            if (size + readPosition_ > buffer_.size())
                throw std::exception("Attempted to read past buffer end!");

            std::string value{buffer_.begin() + readPosition_, buffer_.begin() + readPosition_ + size};
            readPosition_ += size;

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
        auto Size() const
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

        // Returns the data stored in the stream's buffer
        auto Data()
        {
            return buffer_.data();
        }

      private:
        size_t readPosition_ = 0;
        std::vector<uint8_t> buffer_;

        template<typename T>
        struct has_encode_method
        {
        private:
            typedef std::true_type yes;
            typedef std::false_type no;

            template<typename U> static auto test(int) -> decltype(std::declval<U>().Encode() == 1, yes());

            template<typename> static no test(...);

        public:

            static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
        };

        template<typename T>
        struct has_decode_method
        {
        private:
            typedef std::true_type yes;
            typedef std::false_type no;

            template<typename U> static auto test(int) -> decltype(std::declval<U>().Decode() == 1, yes());

            template<typename> static no test(...);

        public:

            static constexpr bool value = std::is_same_v<decltype(test<T>(0)), yes>;
        };
    };
}