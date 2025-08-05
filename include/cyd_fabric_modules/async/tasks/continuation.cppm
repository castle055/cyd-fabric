// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:continuation;

import std;
import reflect;

import fabric.logging;

export import :types;

export namespace fabric::tasks {
  struct continuation_t {
    std::optional<std::shared_ptr<executor>> current_executor;
    std::optional<std::shared_ptr<executor>> caller_executor;
    std::optional<task_handle<>>             cont;

    bool await_ready() const noexcept {
      return not cont.has_value();
    }

    task_handle<> await_suspend(task_handle<> h) noexcept;

    void await_resume() const noexcept {}
  };
} // namespace fabric::tasks
