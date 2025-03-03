// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async:timer_manager;

import std;
import reflect;

export import fabric.async.scheduler;

namespace fabric::async {
  export class timer_manager_t;

  export struct timer_options_t {
    duration interval;
    bool     repeat  = false;
    bool     run_now = false;
  };

  export struct timer_data_t {
    duration              interval;
    bool                  repeat = false;
    std::function<void()> callback;
    std::atomic_flag      stop_flag{false};
  };

  export struct timer_instance_t {
    std::shared_ptr<timer_data_t> data;
    time_point                    next_run;

    bool operator <(const timer_instance_t &other) const {
      return next_run > other.next_run;
    }
  };

  export class timer_t {
    friend timer_manager_t;
    explicit timer_t(const std::shared_ptr<timer_data_t>& ref)
        : ref_(ref) {}

  public:
    void cancel() const {
      if (not ref_.expired()) {
        ref_.lock()->stop_flag.test_and_set();
      }
    }

    void set_interval(duration new_interval) const {
      if (not ref_.expired()) {
        ref_.lock()->interval = new_interval;
      }
    }

  private:
    std::weak_ptr<timer_data_t> ref_;
  };

  class timer_manager_t: public virtual scheduler_t {
  private TEST_PUBLIC: //
    std::priority_queue<timer_instance_t> timers{};
    std::mutex timers_mutex{};

  protected TEST_PUBLIC: /// @name Bus Interface
    void run_timers() {
      std::scoped_lock lock(timers_mutex);
      time_point now = clock::now();

      while (not timers.empty() and timers.top().next_run <= now) {
        auto [data, scheduled_run] = timers.top();
        timers.pop();

        if (not data->stop_flag.test()) {
          data->callback();

          now = clock::now();
          if (data->repeat) {
            while (scheduled_run <= now) {
              scheduled_run += data->interval;
            }
            timers.emplace(data, scheduled_run);
          }
        }
      }

      if (not timers.empty()) {
        set_next_wakeup(timers.top().next_run);
      }
    }

  public: /// @name Public Interface
    timer_t create_timer(timer_options_t opts, std::function<void()> callback) {
      auto data = std::make_shared<timer_data_t>(opts.interval, opts.repeat, callback);

      time_point next_run = opts.run_now ? clock::now() : clock::now() + opts.interval;

      {
        std::scoped_lock lock(timers_mutex);
        timers.emplace(data, next_run);
      }

      set_next_wakeup(next_run);
      this->cv.notify_all();

      return timer_t{data};
    }

  public: /// @name Getter
    timer_manager_t& get_timer_manager() {
        return *this;
    }
  };
}
