// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  observable.cppm
 *! \brief
 *!
 */

export module fabric.tasks:observable;
import :task;
import :task_promise;
import :executor;
import :this_task.get_executor;
import :launch;

import std;
import reflect;

namespace fabric {
  using task_list_t = std::forward_list<std::pair<tasks::executor*, tasks::task_handle<>>>;

  struct observable_awaitable {
    task_list_t& task_list;

    bool await_ready() {
      return false;
    }

    template <typename P>
    void await_suspend(tasks::task_handle<P> h) {
      task_list.emplace_front(h.promise().executor_.get(), h);
    }

    void await_resume() {}
  };

  export template <typename T>
  class observable {
    task_list_t waiting_tasks_{};
    T           value_;

  public:
    template <typename... Args>
      requires requires(Args&&... a) { T{std::forward<Args>(a)...}; }
    observable(Args&&... args)
        : value_(std::forward<Args>(args)...) {}

    operator T&() {
      return value_;
    }

    T& operator*() {
      return value_;
    }

    T* operator->() {
      return &value_;
    }

    void notify() {
      for (const auto& [exec, waiting_task]: waiting_tasks_) {
        exec->schedule_handle(waiting_task);
      }
    }

    template <typename S>
      requires requires(S s) { value_ = std::forward<S>(s); }
    void set(S&& new_val) {
      value_ = std::forward<S>(new_val);
      notify();
    }

    template <typename S>
      requires requires(S s) { value_ = std::forward<S>(s); }
    observable& operator=(S&& new_val) {
      value_ = std::forward<S>(new_val);
      notify();
      return *this;
    }

    task<> until_change() {
      co_await observable_awaitable{waiting_tasks_};
      co_return;
    }

    task<> until_equal(const T& other) {
      while (value_ != other) {
        co_await observable_awaitable{waiting_tasks_};
      }
      co_return;
    }

    task<> until_different(const T& other) {
      while (value_ == other) {
        co_await observable_awaitable{waiting_tasks_};
      }
      co_return;
    }

    task<> until(auto&& predicate) {
      while (not predicate(value_)) {
        co_await observable_awaitable{waiting_tasks_};
      }
      co_return;
    }
  };
} // namespace fabric
