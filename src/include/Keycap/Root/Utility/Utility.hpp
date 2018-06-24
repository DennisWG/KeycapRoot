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

#include <memory>
#include <string>

#include <cctype>
#include <iomanip>

namespace spdlog
{
    class logger;
}

namespace Keycap::Root::Utility
{
    // Sets the console's title to the given title
    extern void SetConsoleTitle(std::string const& title);

    // Returns the logger of the given name if it exists or returns a newly created one with a null_sink
    extern std::shared_ptr<spdlog::logger> GetSafeLogger(std::string const& name);

    template <typename T>
    auto shared_from_that(T* p)
    {
        return std::shared_ptr<T>(p->shared_from_this(), p);
    }

    template <typename Iterator>
    std::string ToHexString(Iterator begin, Iterator end)
    {
        std::stringstream ss;
        ss << std::hex << std::uppercase << std::setfill('0');
        for (; begin != end; ++begin)
            ss << std::setw(2) << static_cast<unsigned int>(*begin);

        return ss.str();
    }

    // Dumps the given range of iterators into the given destination with the given indentation
    // Result:
    // offset                           hex                      ascii
    // 00000000 48 65 6c 6c 6f 2c 20 57  6f 72 6c 64 21          Hello, World!
    template <typename Iterator>
    void DumpAsHex(Iterator begin, Iterator end, std::ostream& destination, int indent = 0)
    {
        unsigned char buffer[16]{};
        int address = 0;

        destination << std::hex << std::setfill('0');

        auto printAddress = [&address, &destination, indent]() {
            for (int j = 0; j < indent; ++j)
                destination << ' ';
            destination << std::setw(8) << address;
        };

        auto printHex = [&buffer, &destination](int n) {
            for (int i = 0; i < n; ++i)
            {
                if (i % 8 == 0)
                    destination << ' ';
                destination << std::setw(2) << static_cast<unsigned int>(buffer[i]) << ' ';
            }
            for (int i = 0; i < (16 - n); ++i)
            {
                if ((16 - n - i) == 8)
                    destination << ' ';
                destination << "   ";
            }
        };

        auto printChars = [&buffer, &destination](int n) {
            for (int i = 0; i < n; ++i)
            {
                if (isprint(buffer[i]))
                    destination << buffer[i];
                else
                    destination << '.';
            }
        };

        int i = 0;
        for (; begin != end; ++begin, ++i)
        {
            if (i >= 16)
            {
                printAddress();
                printHex(16);
                printChars(16);

                address += 16;
                i = 0;
                destination << '\n';
            }

            buffer[i] = *begin;
        }

        if (i != 0)
        {
            printAddress();
            printHex(i);
            printChars(i);
            destination << '\n';
        }

        destination << std::dec;
    }
}