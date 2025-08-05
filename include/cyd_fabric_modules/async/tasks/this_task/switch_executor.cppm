// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  switch_executor.cppm
 *! \brief
 *!
 */

export module fabric.tasks:this_task.switch_executor;
import :task;
import :task_promise;
import :executor;

import std;
import reflect;


export namespace fabric::this_task {
  struct switch_executor {
    fabric::tasks::executor::sptr executor;

    explicit switch_executor(const fabric::tasks::executor::sptr& executor)
        : executor(executor) {}

    bool await_ready() const noexcept {
      return false;
    }
    template <typename P>
    fabric::tasks::task_handle<> await_suspend(fabric::tasks::task_handle<P> h) noexcept {
      bool same_executor = h.promise().executor_ == executor;
      if (same_executor) {
        return h;
      }
      executor->schedule_handle(h);
      return std::noop_coroutine();
    }
    void await_resume() const noexcept {}
  };
} // namespace fabric::this_task
