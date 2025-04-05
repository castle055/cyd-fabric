// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 2/23/25.
//

export module fabric.async.scheduler;

import std;

import fabric.tasks;

using namespace std::chrono_literals;

export namespace fabric::async {
  using clock = tasks::clock;
  using time_point = tasks::time_point;
  using duration = tasks::duration;

  class scheduler_t {
  protected:
    std::shared_ptr<tasks::schedule_t> schedule_{std::make_shared<tasks::schedule_t>()};

    void notify() {
      schedule_->notify();
    }

    void set_next_wakeup(time_point tp) {
      schedule_->set_next_wakeup(tp);
    }

    void reset_next_wakeup() {
      schedule_->reset_next_wakeup();
    }

    void wait() {
      schedule_->wait();
    }
  };
}