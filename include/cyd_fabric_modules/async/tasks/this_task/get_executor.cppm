// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  get_executor.cppm
 *! \brief 
 *!
 */

export module fabric.tasks:this_task.get_executor;
import :task;
import :task_promise;
import :executor;

import std;
import reflect;

struct awaitable_get_executor {
  std::weak_ptr<fabric::tasks::executor> val;

  bool await_ready() const noexcept {
    return false;
  }
  template <typename P>
  bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    val = h.promise().get_executor();
    return false;
  }
  std::weak_ptr<fabric::tasks::executor> await_resume() const noexcept {
    return val;
  }
};


export namespace fabric::this_task {
  task<const tasks::executor&> get_executor() {
    co_return *(co_await awaitable_get_executor{}).lock();
  }

  task<tasks::executor::sptr> get_executor_sptr() {
    co_return (co_await awaitable_get_executor{}).lock();
  }
}
