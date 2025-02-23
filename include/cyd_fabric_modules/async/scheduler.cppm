// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 2/23/25.
//

export module fabric.async.scheduler;

import std;

export namespace fabric::async {
  using clock = std::chrono::steady_clock;
  using time_point = std::chrono::time_point<clock>;
  using duration = typename clock::duration;

  class scheduler_t {
  protected:
    std::condition_variable cv{};
    std::mutex mtx{};

    time_point next_wake_up = clock::now();
    void set_next_wakeup(time_point tp) {
      if (tp < next_wake_up) {
        next_wake_up = tp;
      }
    }
  };
}