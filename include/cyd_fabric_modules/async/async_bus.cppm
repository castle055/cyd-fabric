// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

import std;
export module fabric.async;

export import :event_queue;
export import :coroutine_rt;

export namespace cyd::fabric::async {
    enum class async_bus_status_e {
      RUNNING,
      STOPPED,
    };
    
    class async_bus_t: public event_queue_t,
                       public coroutine_runtime_t {
    public: /// @name Construction & RAII
      // ! Constructor
      async_bus_t() {
        thread_start();
      }
      // ! Destructor
      ~async_bus_t() {
        thread_stop();
      }
      // ! Copy
      async_bus_t(const async_bus_t &rhs) = delete;
      async_bus_t &operator=(const async_bus_t &rhs) = delete;
      // ! Move
      async_bus_t(async_bus_t &&rhs) = delete;
      async_bus_t &operator=(async_bus_t &&rhs) = delete;
    
    private TEST_PUBLIC: /// @name Status
      std::atomic<async_bus_status_e> status_ = async_bus_status_e::STOPPED;
    private TEST_PUBLIC: /// @name Thread
      std::unique_ptr<std::thread> thread_ = nullptr;
      void thread_start() {
        if (!thread_) {
          status_ = async_bus_status_e::RUNNING;
          thread_ = std::make_unique<std::thread>([](async_bus_t* bus) {bus->thread_task();}, this);
        }
      }
      void thread_stop() {
        if (thread_) {
          status_ = async_bus_status_e::STOPPED;
          thread_->join();
          thread_.reset(nullptr);
        }
      }
      
      void thread_task() {
        using namespace std::chrono_literals;
        auto prev_t = std::chrono::system_clock::now();
        while (status_ == async_bus_status_e::RUNNING) {
          prev_t = std::chrono::system_clock::now();
          events_process_batch();
          coroutine_run();
          std::this_thread::sleep_until(prev_t + 100us);
        }
      }
    };
}
