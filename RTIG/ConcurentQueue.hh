// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
//
// This program is free software ; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation ; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// ----------------------------------------------------------------------------

#ifndef CERTI_RTIG_CONCURENTQUEUE_HH
#define CERTI_RTIG_CONCURENTQUEUE_HH

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

namespace certi {
namespace rtig {

template <typename T>
class ConcurentQueue {
public:
    using value_type = T;

    struct pop_timeout_exception {
    };

    ConcurentQueue() = default;

    /**
     * BLOCKING
     */
    T pop()
    {
        std::unique_lock<std::mutex> lock{my_mutex};
        while (my_queue.empty()) {
            my_condition.wait(lock);
        }
        auto ret = my_queue.front();
        my_queue.pop();
        return ret;
    }

    T popTimeout(const std::chrono::seconds timeout = std::chrono::seconds{1})
    {
        std::unique_lock<std::mutex> lock{my_mutex};
        while (my_queue.empty()) {
            if (my_condition.wait_for(lock, timeout) == std::cv_status::timeout) {
                throw pop_timeout_exception{};
            }
        }
        auto ret = my_queue.front();
        my_queue.pop();
        return ret;
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> lock{my_mutex};
        my_queue.push(item);
        lock.unlock();
        my_condition.notify_one();
    }

    void push(T&& item)
    {
        std::unique_lock<std::mutex> lock{my_mutex};
        my_queue.push(std::move(item));
        lock.unlock();
        my_condition.notify_one();
    }

private:
    std::queue<T> my_queue{};
    std::mutex my_mutex{};
    std::condition_variable my_condition{};
};

#endif // CONCURENT_QUEUE_H
}
}

#endif // CERTI_RTIG_CONCURENTQUEUE_HH
