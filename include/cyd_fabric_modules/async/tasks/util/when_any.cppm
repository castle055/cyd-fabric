// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  when_any.cppm
 *! \brief
 *!
 */

export module fabric.tasks:when_any;
import :task;
import :task_promise;
import :executor;
import :this_task.get_executor;
import :observable;

import std;
import reflect;

namespace fabric {
  template <typename T>
  using safe_result_type = std::conditional_t<
    std::is_copy_constructible_v<typename T::return_type>,
    typename T::return_type,
    T>;

  task<> when_task(tasks::Awaitable auto& t, observable<bool>& completion_flag) {
    co_await t;
    // auto res = co_await t;
    if (not completion_flag) {
      completion_flag = true;
    }
  }

  template <typename R>
  task<> when_task(
    task<R> t, observable<bool>& completion_flag, safe_result_type<task<R>>* result = nullptr
  ) {
    if constexpr (std::is_copy_constructible_v<R>) {
      auto res = co_await t;
      if (not completion_flag) {
        if (nullptr != result) {
          *result = res;
        }
        completion_flag = true;
      }
    } else {
      co_await t;
      if (not completion_flag) {
        if (nullptr != result) {
          *result = t;
        }
        completion_flag = true;
      }
    }
  }

  template <typename R>
  void schedule_if_needed(const tasks::executor& exec, const task<R>& task) {
    if (not task.running()) {
      exec.schedule(task);
    }
  }

  void schedule_if_needed(const tasks::executor& exec, const tasks::Awaitable auto& task) {}

  export template <tasks::Awaitable... T>
  task<> when_any(T... tasks) {
    const tasks::executor& exec = co_await this_task::get_executor();
    observable<bool>       completion_flag{false};

    (schedule_if_needed(exec, tasks), ...);
    (co_await fabric::launch(when_task(tasks, completion_flag)), ...);
    co_await completion_flag.until_equal(true);

    co_return;
  }

  export template <typename R, typename... Rs>
    requires(std::is_same_v<R, Rs> and ...)
  task<safe_result_type<task<R>>> when_any(task<R> t, task<Rs>... ts) {
    const tasks::executor&    exec = co_await this_task::get_executor();
    observable<bool>          completion_flag{false};
    safe_result_type<task<R>> result;

    schedule_if_needed(exec, t);
    (schedule_if_needed(exec, ts), ...);

    co_await fabric::launch(when_task(t, completion_flag, &result));
    (co_await fabric::launch(when_task(ts, completion_flag, &result)), ...);

    co_await completion_flag.until_equal(true);

    co_return result;
  }

  export template <typename Iterable, typename T = typename Iterable::value_type>
    requires std::ranges::range<Iterable>
  task<safe_result_type<T>> when_any(const Iterable& ts) {
    const tasks::executor& exec = co_await this_task::get_executor();
    observable<bool>       completion_flag{false};
    safe_result_type<T>    result;

    for (const auto& t: ts) {
      schedule_if_needed(exec, t);
    }

    for (const auto& t: ts) {
      co_await fabric::launch(when_task(t, completion_flag, &result));
    }

    co_await completion_flag.until_equal(true);

    co_return result;
  }
} // namespace fabric
