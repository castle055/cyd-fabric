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

struct awaitable_switch_executor {
  fabric::tasks::executor::sptr val;

  bool await_ready() const noexcept {
    return false;
  }
  template <typename P>
  void await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    val->schedule_handle(h);
  }
  void await_resume() const noexcept {}
};


export namespace fabric::this_task {
  task<> switch_executor(tasks::executor::sptr exec) {
    co_await awaitable_switch_executor{exec};
  }
}
