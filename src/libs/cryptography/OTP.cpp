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

#include <keycap/root/cryptography/OTP.hpp>
#include <keycap/root/utility/numbers.hpp>

#include <botan/base32.h>
#include <botan/hmac.h>

#include <ctime>

namespace keycap::root::cryptography
{
    namespace HOTP
    {
        std::string generate(std::string const& key, uint64_t counter, size_t num_digits)
        {
            auto hmac = Botan::HMAC::create("HMAC(SHA1)");

            hmac->set_key(Botan::base32_decode(key));
            auto text = utility::to_array(counter);
            std::reverse(text.begin(), text.end());
            hmac->update(text.data(), text.size());
            auto hash = hmac->final();

            int offset = hash[hash.size() - 1] & 0x0F;

            // clang-format off
            int binary = ((hash[offset] & 0x7f) << 24)
                | ((hash[offset + 1] & 0xff) << 16)
                | ((hash[offset + 2] & 0xff) << 8)
                | (hash[offset + 3] & 0xff);
            // clang-format on

            int constexpr DIGITS_POWER[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};
            int otp = binary % DIGITS_POWER[num_digits];
            auto result = std::to_string(binary % DIGITS_POWER[num_digits]);

            while (result.size() < num_digits)
                result = '0' + result;

            return result;
        }
    }

    namespace TOTP
    {
        std::string generate(std::string const& key, time_t now, time_t start, time_t step, size_t num_digits)
        {
            uint64_t counter = (now - start) / step;
            return HOTP::generate(key, counter, num_digits);
        }

        bool validate(std::string const& key, std::string const& code)
        {
            const auto time = std::time(nullptr);
            const auto now = static_cast<std::uint64_t>(time);

            for (int i = -1; i < 2; ++i)
            {
                auto step = static_cast<std::uint64_t>((std::floor(now / 30))) + i;
                auto code_n = HOTP::generate(key, step, code.size());

                if (code == code_n)
                    return true;
            }

            return false;
        }
    }

}