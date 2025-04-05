// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:schedule;

import std;
import reflect;

import fabric.logging;
import fabric.concurrent_queue;

export import :types;

using namespace std::chrono_literals;

export namespace fabric::tasks {
  class schedule_t {
    struct delayed_task_t {
      time_point    due;
      task_handle<> task;

      auto operator<(const delayed_task_t& other) const {
        return due > other.due;
      }
    };
    concurrent_queue<delayed_task_t, std::priority_queue> delayed_task_queue_{};
    concurrent_queue<task_handle<>>                       task_queue_{};

    std::condition_variable cv{};
    std::mutex              mtx{};
    std::atomic<time_point> next_wakeup = clock::now();

  public:
    bool empty() const {
      return delayed_task_queue_.empty() and task_queue_.empty();
    }

    template <typename P>
    void enqueue(const task_handle<P>& task_) {
      if (task_.done()) {
        LOG::print{ERROR}("Task is already done.");
        return;
      }
      task_queue_.push(task_);
      notify();
    }

    template <typename P>
    void enqueue_delayed(time_point due, const task_handle<P>& task_) {
      if (task_.done()) {
        LOG::print{ERROR}("Task is already done.");
      }
      delayed_task_queue_.push(delayed_task_t{due, task_});
      set_next_wakeup(due);
    }

    bool run() {
      bool work_left = false;
      if (auto task_opt = task_queue_.try_pop(); task_opt.has_value()) {
        auto& it = task_opt.value();
        try {
          it.resume();
        } catch (const std::exception& e) {
          std::cerr << e.what() << std::endl;
        }
        if (not task_queue_.empty()) {
          work_left = true;
        }
      }
      return work_left;
    }

    void run_all() {
      enque_delayed_tasks();
      while (run())
        ;
    }

    void enque_delayed_tasks() {
      auto now = clock::now();
      if (not delayed_task_queue_.empty()) {
        delayed_task_t t{};
        while (delayed_task_queue_.try_pop(t)) {
          if (t.due <= now) {
            task_queue_.push(t.task);
          } else {
            set_next_wakeup(t.due);
            delayed_task_queue_.push(t); // Push again, because we popped just to read it
            return;
          }
        }
      }
    }

    void set_next_wakeup(time_point tp) {
      if (tp < next_wakeup.load()) {
        next_wakeup.store(tp);
      }
      cv.notify_all();
    }

    void reset_next_wakeup() {
      next_wakeup.store(clock::now() + 3600s);
    }

    void wait() {
      std::unique_lock lock{mtx};
      this->cv.wait_until(lock, this->next_wakeup.load(), [this] {
        return clock::now() >= this->next_wakeup.load();
      });
      lock.unlock();
    }

    void notify() {
      set_next_wakeup(clock::now() - 1s); // wake up now
      cv.notify_all();
    }
  };
} // namespace fabric::tasks
