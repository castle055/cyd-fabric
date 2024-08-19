// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async:coroutine_rt;

import std;

export import :coroutines;

export namespace cyd::fabric::async {
    class coroutine_runtime_t {
    private TEST_PUBLIC:
      std::queue<async_handle<>> coroutine_queue {};
      std::mutex queue_mtx {};
    
    protected TEST_PUBLIC: /// @name Bus Interface
      void coroutine_run() {
        std::optional<async_handle<>> current_handle;
        {
          std::scoped_lock lk {queue_mtx};
          if (coroutine_queue.empty()) {
            current_handle = std::nullopt;
          } else {
            current_handle = coroutine_queue.front();
            coroutine_queue.pop();
          }
        }
        
        current_handle.transform([](auto &&it) {
          it.resume();
          return it;
        });
      }
    public: /// @name Public Interface
      template<typename R>
      // ? Enqueue an already instantiated async handle
      async<R> &coroutine_enqueue(async<R> &handle) {
        std::scoped_lock lk {queue_mtx};
        coroutine_queue.push(handle.h_);
        return handle;
      }
      
      // ? Enqueue anything that is a coroutine, so anything that returns `async<>`
      template<
        typename C,
        typename ...Args,
        typename R = decltype(std::declval<C>()(std::declval<Args>()...))::return_type
      >
      requires requires(C &&c, Args &&...args) {
        {c(std::forward<Args>(args)...)} -> std::convertible_to<async<R>>;
      }
      async<R> coroutine_enqueue(C &&coroutine, Args &&... args) {
        auto a = coroutine(std::forward<Args>(args)...);
        coroutine_enqueue(a);
        return a;
      }
      
      // ? Anything that does not return `async<>` fails this static assertion
      template<typename C, typename ...Args, typename R = decltype(std::declval<C>()(std::declval<Args>()...))>
      inline void coroutine_enqueue(C &&coroutine, Args &&... args) {
        static_assert(false, "`coroutine` must return type async<>.");
      }
    public: /// @name Getter
      coroutine_runtime_t &get_coroutine_runtime() {
        return *this;
      }
    };
}
