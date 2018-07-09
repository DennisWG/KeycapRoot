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
/*
    meta_counter taken from https://github.com/DaemonSnake/unconstexpr

    MIT License

    Copyright (c) 2017 Bastien Penavayre

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

namespace keycap::root::utility
{
    namespace detail
    {
        template <class Tag = void, class Type = int, Type Start = 0, Type Step = 1>
        class meta_counter
        {
            template <Type N>
            struct Flag
            {
                friend constexpr bool adl_flag(Flag<N>);
            };

            template <Type N>
            struct Writer
            {
                friend constexpr bool adl_flag(Flag<N>)
                {
                    return true;
                }
                static constexpr Type value = N;
            };

            template <Type N, bool = adl_flag(Flag<N>{})>
            static constexpr Type reader(int, Flag<N>, Type r = reader(0, Flag<N + Step>{}))
            {
                return r;
            }

            template <Type N>
            static constexpr Type reader(float, Flag<N>)
            {
                return N;
            }

          public:
            static constexpr Type value(Type r = reader(0, Flag<Start>{}))
            {
                return r;
            }

            template <Type Value = value()>
            static constexpr Type next(Type r = Writer<Value>::value)
            {
                return r + Step;
            }
        };

        template <unsigned>
        struct unique_type
        {
        };
    }

    class uniq_value
    {
        struct Detail
        {
            template <unsigned N>
            struct Flag
            {
                friend constexpr bool adl_flag(Flag<N>);
            };

            template <unsigned N>
            struct Writer
            {
                friend constexpr bool adl_flag(Flag<N>)
                {
                    return true;
                }

                static constexpr unsigned value = N;
            };

            template <unsigned N = 0, bool = adl_flag(Flag<N>{})>
            static constexpr unsigned reader(int, unsigned r = reader<N + 1>(0))
            {
                return r;
            }

            template <unsigned N = 0>
            static constexpr unsigned reader(float)
            {
                return Writer<N>::value;
            }
        };

      public:
        template <unsigned N = Detail::reader(0)>
        static constexpr unsigned value = N;
    };

    template <class Type = int, Type Start = 0, Type It = 1, unsigned I = uniq_value::value<>>
    using meta_counter = detail::meta_counter<detail::unique_type<I>, Type, Start, It>;

    // taken from https://stackoverflow.com/a/45365798
    namespace detail
    {
        template <typename Callable>
        union storage
        {
            storage()
            {
            }
            std::decay_t<Callable> callable;
        };

        template <int, typename Callable, typename Ret, typename... Args>
        auto fnptr_(Callable&& c, Ret (*)(Args...))
        {
            static bool used = false;
            static storage<Callable> s;
            using type = decltype(s.callable);

            if (used)
                s.callable.~type();
            new (&s.callable) type(std::forward<Callable>(c));
            used = true;

            return [](Args... args) -> Ret { return Ret(s.callable(args...)); };
        }
    }

    template <typename Fn, typename Callable>
    Fn* fnptr(Callable&& c)
    {
        using counter = meta_counter<>;
        return detail::fnptr_<counter::next()>(std::forward<Callable>(c), (Fn*)nullptr);
    }
}