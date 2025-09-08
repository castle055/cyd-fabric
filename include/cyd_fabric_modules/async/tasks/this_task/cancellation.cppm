// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  cancellation.cppm
 *! \brief
 *!
 */

export module fabric.tasks:this_task.cancellation;
import :task;
import :task_promise;

import std;
import reflect;

struct awaitable_is_cancelled {
  bool cancelled{false};
  bool await_ready() const noexcept {
    return false;
  }
  template <typename P>
  fabric::tasks::task_handle<P> await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    cancelled = h.promise().is_cancelled();
    return h;
  }
  bool await_resume() const noexcept {
    return cancelled;
  }
};

struct awaitable_until_cancelled {
  fabric::tasks::task_promise_base* promise;

  bool await_ready() const noexcept {
    return promise->is_cancelled();
  }
  template <typename P>
  void await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    promise->await_cancellation(h);
  }
  void await_resume() const noexcept {}
};


export namespace fabric::this_task {
  awaitable_is_cancelled is_cancelled() {
    return awaitable_is_cancelled{};
  }

  awaitable_until_cancelled until_cancelled() {
    const auto t = this_executor::get().get_schedule()->get_current_task().value();
    auto&      promise =
      std::coroutine_handle<tasks::task_promise_base>::from_address(t.address()).promise();
    return awaitable_until_cancelled{&promise};
  }
} // namespace fabric::this_task
