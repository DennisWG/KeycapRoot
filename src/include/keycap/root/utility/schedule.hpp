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

#include <boost/asio.hpp>

#include <atomic>
#include <chrono>
#include <mutex>
#include <unordered_map>

namespace keycap::root::utility
{
    class schedule
    {
        class scheduled_item
        {
          private:
            friend class schedule;
            uint32_t counter_ = 0;
            scheduled_item(uint32_t counter)
              : counter_{counter}
            {
            }
        };

      public:
        schedule(int thread_count = 1)
          : work_{io_service_}
        {
            for (int i = 0; i < thread_count; ++i)
                thread_pool_.emplace_back([=] {
                    try
                    {
                        io_service_.run();
                    }
                    catch (std::exception const&)
                    {
                        io_service_.run();
                    }
                });
        }

        ~schedule()
        {
            cancel_all();

            io_service_.stop();

            for (auto& thread : thread_pool_)
            {
                if (thread.joinable())
                    thread.join();
            }
        }

        // Adds the given callback to the schedule. Will be called after the given duration. Returns a scheduled_item
        // allowing you to stop the callback early, if needed. An optional boost::asio::io_service may be passed
        // so the callback will be called on the io_service's thread pool.
        template <typename Rep, typename Period, typename Callback>
        scheduled_item
        add(std::chrono::duration<Rep, Period> duration, Callback&& callback,
            boost::asio::io_service* io_service = nullptr)
        {
            auto counter = counter_++;

            std::lock_guard<std::mutex> lock{timers_mutex_};
            auto& timer
                = timers_.try_emplace(counter, io_service_, std::chrono::steady_clock::now() + duration).first->second;

            timer.async_wait(
                [ this, counter, callback = std::forward<Callback>(callback),
                  io_service = std::move(io_service) ](boost::system::error_code error) {
                    if (timers_.find(counter) == timers_.end())
                        return;

                    cancel(scheduled_item{counter});

                    if (io_service)
                        io_service->post([callback, error]() { callback(error); });
                    else
                        callback(error);
                });

            return scheduled_item{counter};
        }

        // Cancels the given scheduled_item
        void cancel(scheduled_item item)
        {
            std::lock_guard<std::mutex> lock{timers_mutex_};
            if (auto itr = timers_.find(item.counter_); itr != timers_.end())
            {
                itr->second.cancel();
                timers_.erase(itr);
            }
        }

        // Cancels all scheduled callbacks
        void cancel_all()
        {
            std::lock_guard<std::mutex> lock{timers_mutex_};
            for (auto & [ _, timer ] : timers_)
                timer.cancel();
        }

      private:
        boost::asio::io_service io_service_;
        boost::asio::io_service::work work_;
        std::vector<std::thread> thread_pool_;

        std::atomic_uint32_t counter_ = 0;
        std::unordered_map<uint32_t, boost::asio::steady_timer> timers_;

        std::mutex timers_mutex_;
    };
}