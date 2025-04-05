// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/async_events.h>

export module fabric.async;

import std;
import fabric.logging;
export import fabric.async.ebus;
export import fabric.tasks;
export import :timers;

export namespace fabric::async {
  EVENT(StopBusEvent){};

  class async_bus_t: //
                     public tasks::executor::sptr,
                     public ebus {
  public: /// @name Construction & RAII
    // ! Constructor
    async_bus_t()
        : tasks::executor::sptr(tasks::executor::make()),
          event_processing_task_(get()->schedule(ebus::event_processing_task())),
          stop_bus_listener(on_event([&](const StopBusEvent& ev) -> task<> {
            get()->request_stop();
            LOG::print {DEBUG}("Stop Bus Event received. Executor stop requested.");
            co_return;
          })) {}
    // ! Copy
    async_bus_t(const async_bus_t& rhs)            = delete;
    async_bus_t& operator=(const async_bus_t& rhs) = delete;
    // ! Move
    async_bus_t(async_bus_t&& rhs)                 = delete;
    async_bus_t& operator=(async_bus_t&& rhs)      = delete;

    ~async_bus_t() {
      get()->join();
    }

    timer_t create_timer(timer_options_t opts, std::function<task<>()> callback) {
      auto data = std::make_shared<timer_data_t>(opts.interval, opts.repeat, callback);

      if (opts.run_now) {
        get()->schedule(timer_task, data);
      } else {
        get()->schedule(tasks::clock::now() + opts.interval, timer_task, data);
      }

      return timer_t{data};
    }

    timer_t create_timer(timer_options_t opts, std::function<void()> callback) {
      const auto data = std::make_shared<timer_data_t>(
        opts.interval,
        opts.repeat,
        [callback = std::move(callback)] -> task<> {
          callback();
          co_return;
        }
      );

      if (opts.run_now) {
        get()->schedule(timer_task, data);
      } else {
        get()->schedule(tasks::clock::now() + opts.interval, timer_task, data);
      }

      return timer_t{data};
    }

  private:
    task<>                 event_processing_task_;
    listener<StopBusEvent> stop_bus_listener;
  };
} // namespace fabric::async
