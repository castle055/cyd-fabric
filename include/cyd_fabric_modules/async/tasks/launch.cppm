// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  launch.cppm
 *! \brief
 *!
 */

export module fabric.tasks:launch;
import :task;
import :task_promise;
import :executor;
import :this_task.get_executor;

import std;
import reflect;

export namespace fabric {
  template <typename P>
  fabric::task<> launch(tasks::task_handle<P>&& task) {
    const tasks::executor& exec = co_await this_task::get_executor();
    exec.schedule_handle(std::move(task));
    // co_yield 123;
    co_return;
  }

  template <typename P>
  fabric::task<> launch(const tasks::task_handle<P>& task) {
    const tasks::executor& exec = co_await this_task::get_executor();
    exec.schedule_handle(task);
    // co_yield 123;
    co_return;
  }

  template <template <typename> typename Container = std::list>
  fabric::task<> launch(const Container<tasks::task_handle<>>& tasks) {
    const tasks::executor& exec = co_await this_task::get_executor();
    for (const auto& task: tasks) {
      exec.schedule_handle(task);
    }
    // co_yield 123;
    co_return;
  }

  fabric::task<> launch(const std::list<tasks::task_handle<>>& tasks) {
    return launch<std::list>(tasks);
  }

  // template <typename P>
  // fabric::task<fabric::task<P>> launch(task<P> task) {
  //   const tasks::executor& exec = co_await this_task::get_executor();
  //   auto                   t    = exec.schedule(std::move(task));
  //   co_yield 123;
  //   co_return t;
  // }

  template <typename... Args>
  auto launch(auto&& task, Args&&... args)
    -> fabric::task<decltype(task(std::forward<Args>(args)...))> {
    const tasks::executor& exec = co_await this_task::get_executor();
    auto                   t    = exec.schedule(std::move(task), std::forward<Args>(args)...);
    // co_yield 123;
    co_return t;
  }
} // namespace fabric
