// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  sleep.cppm
 *! \brief
 *!
 */

export module fabric.tasks:this_task.sleep;
import :task;
import :task_promise;
import :executor;

import std;
import reflect;

struct awaitable_sleep {
  fabric::tasks::duration       delay;
  fabric::tasks::executor::sptr exec;

  bool await_ready() const noexcept {
    return delay == fabric::tasks::duration::zero();
  } /// Always suspend!

  template <typename P>
  void await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    exec = h.promise().get_executor().lock();
    h.promise().reschedule(delay);
  }

  void await_resume() const noexcept {}
};


export namespace fabric::this_task {
  task<> sleep(auto duration) {
    co_await awaitable_sleep{duration};
  }
} // namespace fabric::this_task

// export awaitable_sleep operator co_await(fabric::tasks::duration delay) {
//   return awaitable_sleep{delay};
// }

export awaitable_sleep operator co_await(std::chrono::duration<long double> delay) {
  return awaitable_sleep{std::chrono::duration_cast<fabric::tasks::duration>(delay)};
}

export awaitable_sleep operator co_await(fabric::tasks::time_point time_point) {
  auto now = fabric::tasks::clock::now();
  auto dur = time_point - now;
  if (dur >= decltype(dur)::zero()) {
    return awaitable_sleep{dur};
  }
  return awaitable_sleep{0ms};
}
