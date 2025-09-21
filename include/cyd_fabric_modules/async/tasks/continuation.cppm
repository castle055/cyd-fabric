// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:continuation;

import std;
import reflect;

import fabric.logging;

export import :types;

export namespace fabric::tasks {
  struct continuation_t {
    executor*     executor;
    task_handle<> handle;
  };

  struct continuation_list_t {
    executor*                         current_executor{nullptr};
    std::forward_list<continuation_t> continuations{};
    bool                              detached{false};

    void emplace_continuation(executor* exec, task_handle<> handle) {
      continuations.emplace_front(exec, handle);
    }

    bool await_ready() const noexcept {
      return false;
    }

    void await_suspend(task_handle<> h) noexcept;

    void await_resume() const noexcept {}
  };
} // namespace fabric::tasks
