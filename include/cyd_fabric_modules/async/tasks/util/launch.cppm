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
  task<> launch(tasks::task_handle<P>&& task) {
    const tasks::executor& exec = co_await this_task::get_executor();
    exec.schedule_handle(std::move(task));
    // co_yield 123;
    co_return;
  }

  template <typename P>
  task<> launch(const tasks::task_handle<P>& task) {
    const tasks::executor& exec = co_await this_task::get_executor();
    exec.schedule_handle(task);
    // co_yield 123;
    co_return;
  }

  template <typename R>
  void launch(const task<R>& task) {
    const tasks::executor& exec = this_executor::get();
    exec.schedule(task);
  }

  template <typename R>
  task<R> launch(task<R>&& task_) {
    const tasks::executor& exec = this_executor::get();
    return exec.schedule(std::forward<task<R>>(task_));
  }

  template <template <typename> typename Container = std::list>
  task<> launch(const Container<tasks::task_handle<>>& tasks) {
    const tasks::executor& exec = co_await this_task::get_executor();
    for (const auto& task: tasks) {
      exec.schedule_handle(task);
    }
    // co_yield 123;
    co_return;
  }

  task<> launch(const std::list<tasks::task_handle<>>& tasks) {
    return launch<std::list>(tasks);
  }

  // template <typename P>
  // task<task<P>> launch(task<P> task) {
  //   const tasks::executor& exec = co_await this_task::get_executor();
  //   auto                   t    = exec.schedule(std::move(task));
  //   co_yield 123;
  //   co_return t;
  // }

  template <typename... Args>
  auto launch(auto&& task, Args&&... args)
    -> fabric::task<decltype(task(std::forward<Args>(args)...))> {
    const tasks::executor& exec = co_await this_task::get_executor();
    co_return exec.schedule(task(std::forward<Args>(args)...));
  }
} // namespace fabric
