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

using namespace std::chrono_literals;

export namespace fabric::async {
  EVENT(StopBusEvent){};

  class async_bus_t: //
                     public tasks::executor::sptr,
                     public ebus {
  public: /// @name Construction & RAII
    using sptr = std::shared_ptr<async_bus_t>;

    // ! Constructor
    async_bus_t()
        : tasks::executor::sptr(tasks::executor::make()),
          event_processing_task_(get()->schedule(ebus::event_processing_task()).detach()),
          stop_bus_listener(on_event([&](const StopBusEvent& ev) -> task<> {
            event_processing_task_.cancel();
            co_await event_processing_task_;
            get()->request_stop();
            LOG::print{DEBUG}("Stop Bus Event received. Executor stop requested.");
            co_return;
          })) {
      get()
        ->schedule([] -> task<> {
          LOG::print{DEBUG}("Bus Ready");
          co_return;
        }())
        .detach();
    }
    // ! Copy
    async_bus_t(const async_bus_t& rhs)            = delete;
    async_bus_t& operator=(const async_bus_t& rhs) = delete;
    // ! Move
    async_bus_t(async_bus_t&& rhs)                 = delete;
    async_bus_t& operator=(async_bus_t&& rhs)      = delete;

    ~async_bus_t() {
      event_processing_task_.cancel();
    }

    timer_t create_timer(timer_options_t opts, auto callback) {
      const auto data = std::make_shared<timer_data_t>(opts.interval, opts.repeat, callback);

      if (opts.run_now) {
        get()->schedule(timer_task(data)).detach();
      } else {
        get()->schedule(timer_task(data), tasks::clock::now() + opts.interval).detach();
      }

      return timer_t{data};
    }

    // private:
    //   struct timer_function_wraper_task {
    //     std::function<void()> callback;
    //     task<>                operator()() {
    //       LOG::print{INFO}("TIMER HERE!!! AYEE");
    //       callback();
    //       co_return;
    //     }
    //   };
    //
    // public:
    //   timer_t create_timer(timer_options_t opts, std::function<void()> callback) {
    //     const auto data = std::make_shared<timer_data_t>(
    //       opts.interval,
    //       opts.repeat,
    //       std::function<task<>()>{timer_function_wraper_task{std::move(callback)}}
    //     );
    //
    //     if (opts.run_now) {
    //       get()->schedule(timer_task, data);
    //       LOG::print{DEBUG}("Timer scheduled immediately");
    //     } else {
    //       get()->schedule(tasks::clock::now() + opts.interval, timer_task, data);
    //       LOG::print{DEBUG}("Timer scheduled with delay");
    //     }
    //
    //     return timer_t{data};
    //   }

    tasks::executor::sptr operator->() {
      return *static_cast<tasks::executor::sptr*>(this);
    }

    tasks::executor::sptr get_executor() {
      return *static_cast<tasks::executor::sptr*>(this);
    }

  private:
    detached_task          event_processing_task_;
    listener<StopBusEvent> stop_bus_listener;
  };
} // namespace fabric::async
