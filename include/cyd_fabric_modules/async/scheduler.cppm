// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 2/23/25.
//

export module fabric.async.scheduler;

import std;

using namespace std::chrono_literals;

export namespace fabric::async {
  using clock = std::chrono::steady_clock;
  using time_point = std::chrono::time_point<clock>;
  using duration = typename clock::duration;

  class scheduler_t {
  protected:
    std::condition_variable cv{};
    std::mutex mtx{};

    void notify() {
      set_next_wakeup(clock::now());
      cv.notify_all();
    }

    std::mutex next_wakeup_mutex{};
    time_point next_wakeup = clock::now();
    void set_next_wakeup(time_point tp) {
      std::scoped_lock lock(next_wakeup_mutex);
      if (tp < next_wakeup) {
        next_wakeup = tp;
      }
    }
  };
}