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

  template <typename Ret>
  class task_promise_base;

  template <typename Ret>
  class task_promise_t;

  class executor;
} // namespace fabric::tasks
