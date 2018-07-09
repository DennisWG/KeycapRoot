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

#include <exception>

namespace keycap::root::utility
{
    // A view into an array. Does NOT own the memory!
    template <typename T>
    class array_view
    {
      public:
        array_view(T* ptr, size_t size)
            : ptr_(ptr)
            , size_(size)
        {
        }

        T* begin()
        {
            return ptr_;
        }

        T* end()
        {
            return ptr_ + size_;
        }

        const T* cbegin()
        {
            return ptr_;
        }

        const T* cend()
        {
            return ptr_ + size_;
        }

        T* data()
        {
            return ptr_;
        }

        size_t size()
        {
            return size_;
        }

        T& operator[](size_t index)
        {
            if (index >= size_ || index < 0)
                throw std::exception("index out of bounds");

            return ptr_[index];
        }

      private:
        T* ptr_ = nullptr;
        size_t size_ = 0;
    };

    template <typename T>
    array_view<T> make_array_view(T* ptr, size_t size)
    {
        return array_view<T>{ptr, size};
    }
}