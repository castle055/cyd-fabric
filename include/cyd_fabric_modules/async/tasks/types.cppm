// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:types;

import std;
import reflect;

using namespace std::chrono_literals;

export namespace fabric {
  template <typename = void>
  struct task;
}
export namespace fabric::tasks {
  using clock      = std::chrono::steady_clock;
  using time_point = clock::time_point;
  using duration   = clock::duration;

  template <typename T = void>
  using task_handle = std::coroutine_handle<T>;

  template <typename T>
  class task_result_storage;

  class task_promise_base;

  template <typename Ret>
  class task_promise_t;

  class executor;

  template <typename T, typename = void>
  struct has_co_await: std::false_type {};

  template <typename T>
  struct has_co_await<T, std::void_t<decltype(std::declval<T&>().operator co_await())>>
      : std::true_type {};

  template <typename T, typename = void>
  struct has_free_co_await: std::false_type {};

  template <typename T>
  struct has_free_co_await<T, std::void_t<decltype(operator co_await(std::declval<T&>()))>>
      : std::true_type {};

  template <typename T>
  concept Awaitable = requires(T t) {
    // direct awaitable: has await_ready/await_suspend/await_resume
    { t.await_ready() } -> std::convertible_to<bool>;
    t.await_suspend(std::coroutine_handle<>{});
    t.await_resume();
  } || has_co_await<T>::value || has_free_co_await<T>::value;
} // namespace fabric::tasks
