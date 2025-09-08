// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  when_all.cppm
 *! \brief
 *!
 */

export module fabric.tasks:when_all;
import :task;
import :task_promise;
import :executor;
import :this_task.get_executor;

import std;
import reflect;

namespace fabric {
  template <typename T>
  using safe_result_type = std::conditional_t<
    std::is_copy_constructible_v<typename T::return_type>,
    typename T::return_type,
    T>;

  template <typename R>
  task<safe_result_type<task<R>>> when_task(task<R> task) {
    if constexpr (std::is_copy_constructible_v<R>) {
      co_return co_await task;
    } else {
      co_await task;
      co_return task;
    }
  }

  template <typename R>
  void schedule_if_needed(const tasks::executor& exec, const task<R>& task) {
    if (not task.running()) {
      exec.schedule(task);
    }
  }

  export template <typename... R>
  task<std::tuple<safe_result_type<task<R>>...>> when_all(task<R>... tasks) {
    const tasks::executor& exec = co_await this_task::get_executor();

    (schedule_if_needed(exec, tasks), ...);
    std::tuple<safe_result_type<task<R>>...> task_result{(co_await when_task(tasks))...};

    co_return task_result;
  }

  export template <typename R, typename... Rs>
    requires(std::is_same_v<R, Rs> and ...)
  task<std::array<safe_result_type<task<R>>, 1 + sizeof...(Rs)>>
  when_all(task<R> t, task<Rs>... ts) {
    const tasks::executor& exec = co_await this_task::get_executor();

    schedule_if_needed(exec, t);
    (schedule_if_needed(exec, ts), ...);
    std::array<safe_result_type<task<R>>, 1 + sizeof...(Rs)> task_result{
      co_await when_task(t), (co_await when_task(ts))...
    };

    co_return task_result;
  }

  export template <
    typename Iterable,
    typename T = typename Iterable::value_type>
    requires std::ranges::range<Iterable>
  task<std::vector<safe_result_type<T>>> when_all(const Iterable& ts) {
    const tasks::executor&           exec = co_await this_task::get_executor();
    std::vector<safe_result_type<T>> results;
    results.reserve(ts.size());

    for (const auto& t: ts) {
      schedule_if_needed(exec, t);
    }

    for (const auto& t: ts) {
      results.emplace_back(co_await when_task(t));
    }

    co_return results;
  }

  export task<> when_all_ready(auto&&... tasks) {
    co_await when_all(tasks...);
  }
} // namespace fabric
