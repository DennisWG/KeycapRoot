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

#define QUICK_SCOPE_EXIT(name, F) \
struct name { \
    ~name() { F(); } \
} name##_

namespace impl
{
    template <typename F>
    struct ScopeExit {
        ScopeExit(F f) : f(f) {}
        ~ScopeExit() { f(); }
        F f;
    };

    template <typename F>
    ScopeExit<F> MakeScopeExit(F f) {
        return ScopeExit<F>(f);
    };
}

#define SCOPE_EXIT(name, F) \
    auto name = impl::MakeScopeExit(F)