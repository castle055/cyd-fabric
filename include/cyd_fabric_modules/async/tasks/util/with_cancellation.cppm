// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  with_cancellation.cppm
 *! \brief
 *!
 */

export module fabric.tasks:with_cancellation;
import :task;
import :task_promise;
import :this_task.cancellation;
import :when_any;

import std;
import reflect;

namespace fabric {
  export template <typename Ret>
  task<Ret> with_cancellation(const task<Ret>& task) {
    auto cancellation = this_task::until_cancelled();
    return [](fabric::task<Ret> task, decltype(cancellation) cancellation) -> fabric::task<Ret> {
      co_await fabric::when_any(task, cancellation);
      task.cancel();
      co_return co_await task;
    }(task, cancellation);
  }

  export template <typename Ret>
  task<Ret> with_cancellation(task<Ret>&& task) {
    auto cancellation = this_task::until_cancelled();
    return [](fabric::task<Ret> task, decltype(cancellation) cancellation) -> fabric::task<Ret> {
      co_await fabric::when_any(task, cancellation);
      task.cancel();
      co_return co_await std::move(task);
    }(task, cancellation);
  }
} // namespace fabric
