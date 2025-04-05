// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async:timers;

import std;
import reflect;

export import fabric.tasks;

namespace fabric::async {
  export class timer_manager_t;

  export struct timer_options_t {
    tasks::duration interval;
    bool            repeat  = false;
    bool            run_now = false;

    timer_options_t(const tasks::duration interval_): interval(interval_) {}
    timer_options_t(const tasks::time_point when): interval(when - tasks::clock::now()) {}
  };

  export struct timer_data_t {
    tasks::duration         interval;
    bool                    repeat = false;
    std::function<task<>()> callback;
    std::atomic_flag        stop_flag{false};
  };

  export class timer_t {
    friend timer_manager_t;

  public:
    explicit timer_t(const std::shared_ptr<timer_data_t>& ref)
        : ref_(ref) {}

    void cancel() const {
      if (not ref_.expired()) {
        ref_.lock()->stop_flag.test_and_set();
      }
    }

    void set_interval(tasks::duration new_interval) const {
      if (not ref_.expired()) {
        ref_.lock()->interval = new_interval;
      }
    }

  private:
    std::weak_ptr<timer_data_t> ref_;
  };

  task<> timer_task(const std::shared_ptr<timer_data_t>& timer_data) {
    auto previous_run = tasks::clock::now();
    co_await timer_data->callback();
    auto runtime = tasks::clock::now() - previous_run;

    co_await (timer_data->interval - runtime);

    while (timer_data->repeat and not timer_data->stop_flag.test()) {
      previous_run = tasks::clock::now();
      co_await timer_data->callback();
      runtime = tasks::clock::now() - previous_run;

      co_await (timer_data->interval - runtime);
    }

    co_return;
  }
} // namespace fabric::async
