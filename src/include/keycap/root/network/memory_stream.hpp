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
#include <optional>
#include <type_traits>
#include <vector>

namespace keycap::root::network
{
    // A FIFO stream container
    class memory_stream
    {
      public:
        memory_stream() = default;
        memory_stream(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end)
          : buffer_(begin, end)
        {
        }

        // Puts a T into the stream
        template <typename T>
        void put(T const& value)
        {
            if constexpr (has_encode_method<T>::value)
            {
                return T::Encode(*this);
            }
            gsl::span<uint8_t const> data{reinterpret_cast<uint8_t const*>(&value), sizeof(T)};
            buffer_.insert(buffer_.end(), data.begin(), data.end());
        }

        template <typename T>
        void put(std::vector<T> const& vec)
        {
            for (auto&& element : vec)
                put(element);
        }

        template <typename T, size_t NumElements>
        void put(std::array<T, NumElements> value)
        {
            gsl::span<uint8_t const> data{reinterpret_cast<uint8_t const*>(value.data()),
                                          reinterpret_cast<uint8_t const*>(value.data() + NumElements)};

            buffer_.insert(buffer_.end(), data.begin(), data.end());
        }

        // Puts a single byte into the stream
        void put(uint8_t value)
        {
            buffer_.push_back(value);
        }

        // Puts an arbitrary span into the stream
        template <typename T>
        void put(gsl::span<T> data)
        {
            gsl::span<uint8_t const> d{reinterpret_cast<uint8_t const*>(data.data()), data.length_bytes()};
            buffer_.insert(buffer_.end(), d.begin(), d.end());
        }

        // Puts a span of bytes into the stream
        void put(gsl::span<uint8_t> data)
        {
            [[maybe_unused]] auto end = buffer_.insert(buffer_.end(), data.begin(), data.end());
        }

        // Puts a std::string into the stream
        void put(std::string const& string)
        {
            gsl::span<uint8_t const> d{reinterpret_cast<uint8_t const*>(string.data()),
                                       reinterpret_cast<uint8_t const*>(string.data() + string.size())};
            [[maybe_unused]] auto end = buffer_.insert(buffer_.end(), d.begin(), d.end());
        }

        // Appends a MemoryStream into the stream
        void put(memory_stream const& stream)
        {
            [[maybe_unused]] auto end = buffer_.insert(buffer_.end(), stream.buffer_.begin(), stream.buffer_.end());
        }

        uint8_t& operator[](size_t index)
        {
            if (!has_remaining(index))
                throw std::exception("Tried to override past the stream's end!");

            return buffer_[index + read_position_];
        }

        // Overrides the given value at the given position
        template <typename T>
        void override(T const& value, size_t position)
        {
            if ((position + sizeof(T)) > buffer_.size())
                throw std::exception("Tried to override past the stream's end!");

            *reinterpret_cast<T*>(buffer_.data() + position) = value;
        }

        // Gets a T from the stream
        template <typename T>
        T get()
        {
            if constexpr (has_decode_method<T>::value)
            {
                return T::Decode(*this);
            }
            else
            {
                if (!has_remaining(sizeof(T)))
                    throw std::exception("Attempted to read past buffer end!");

                T value = *reinterpret_cast<T const*>(buffer_.data() + read_position_);
                read_position_ += sizeof(T);

                return value;
            }
        }

        // Gets an array from the stream
        template <typename T, size_t NumElements>
        std::array<T, NumElements> get()
        {
            std::array<T, NumElements> array;

            for (int i = 0; i < NumElements; ++i)
            {
                if constexpr (std::is_same_v<T, std::string>)
                    array[i] = get_string();
                else
                    array[i] = get<T>();
            }

            return array;
        }

        // Returns a std::string with the given size from the stream
        std::string get_string(size_t size)
        {
            if (!has_remaining(size))
                throw std::exception("Attempted to read past buffer end!");

            std::string string = {buffer_.begin() + read_position_, buffer_.begin() + read_position_ + size};
            read_position_ += size;
            return string;
        }

        // Returns a std::string from the stream. Assumes the string is zero-terminated
        std::string get_string()
        {
            std::string str;
            while (read_position_ < buffer_.size())
            {
                char c = get<char>();
                if (c == '\0')
                    break;

                str += c;
            }

            return str;
        }

        // Returns the remaining data as a stream
        memory_stream get_remaining()
        {
            auto old_poisition = read_position_;
            read_position_ = buffer_.size();
            return memory_stream(buffer_.begin() + old_poisition, buffer_.end());
        }

        // Peeks for the given T at the given position in the stream
        template <typename T>
        T peek(size_t where = 0) const
        {
            if (!has_remaining(sizeof(T) + where))
                throw std::exception("Attempted to read past buffer end!");

            return *reinterpret_cast<T const*>(buffer_.data() + where + read_position_);
        }

        // Returns the number of available bytes in the stream
        auto size() const
        {
            return buffer_.size() - read_position_;
        }

        // Removes all elements from the buffer up to the stream's current read position
        void shrink()
        {
            if (read_position_ == 0)
                return;

            [[maybe_unused]] auto end = buffer_.erase(buffer_.begin(), buffer_.begin() + read_position_);
            read_position_ = 0;
        }

        // Returns the data stored in the stream's buffer
        auto data()
        {
            return buffer_.data();
        }

        auto data() const
        {
            return buffer_.data();
        }

        // Clears the memory_stream's buffer and resets all internal state
        void clear()
        {
            buffer_.clear();
            read_position_ = 0;
        }

        // Returns the remaining memory_stream's buffer as a vector of bytes and marks the stream as read.
        auto to_vector()
        {
            auto read_position = read_position_;
            read_position_ = buffer_.size();
            return std::vector<uint8_t>{buffer_.end() - (buffer_.size() - read_position), buffer_.end()};
        }

        // Returns the memory_stream's buffer
        auto& buffer() const
        {
            return buffer_;
        }

        // Decompresses the given length of the buffer
        // May resize (and therefore reallocate) the buffer
        // Returns the size of the decompressed data
        size_t decompress(uint32_t length);

        bool has_data_remaining() const;

        // Writes the given amount of num_bytes to the given desitnation iterator
        template <typename ITER>
        void write_to(ITER destination, std::optional<size_t> num_bytes = {})
        {
            auto begin = std::begin(buffer_);
            auto end = std::end(buffer_);

            if (num_bytes)
            {
                if (!has_remaining(*num_bytes))
                    throw std::exception("Attempted to read past buffer end!");
                end = begin + read_position_ + num_bytes.value();
            }

            std::copy(begin + read_position_, end, destination);
            read_position_ += end - (begin + read_position_);
        }

      private:
        size_t read_position_ = 0;
        std::vector<uint8_t> buffer_;

        bool has_remaining(size_t num_bytes) const
        {
            return read_position_ < buffer_.size();
        }

        template <typename T>
        struct has_encode_method
        {
          private:
            typedef std::true_type yes;
            typedef std::false_type no;

            template <typename U>
            static auto test(int) -> decltype(std::declval<U>().encode() == 1, yes());

            template <typename>
            static no test(...);

          public:
            static constexpr bool value = std::is_same<decltype(test<T>(0)), yes>::value;
        };

        template <typename T>
        struct has_decode_method
        {
          private:
            typedef std::true_type yes;
            typedef std::false_type no;

            template <typename U>
            static auto test(int) -> decltype(std::declval<U>().decode() == 1, yes());

            template <typename>
            static no test(...);

          public:
            static constexpr bool value = std::is_same_v<decltype(test<T>(0)), yes>;
        };
    };
}
