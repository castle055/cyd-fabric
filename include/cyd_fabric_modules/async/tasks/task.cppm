// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:task;

import std;
import reflect;

import fabric.logging;

export import :types;

using namespace std::chrono_literals;

export namespace fabric {
  template <typename Ret = void>
  struct task {
    using promise_type = tasks::task_promise_t<Ret>;
    using handle_type  = tasks::task_handle<promise_type>;

  private:
    std::future<Ret>                 future_;
    tasks::task_handle<promise_type> h_;

  public:
    using return_type = Ret;

    task(tasks::task_handle<promise_type>&& h__, std::future<Ret>&& future__)
        : future_(std::move(future__)),
          h_(h__) {}


    operator tasks::task_handle<promise_type>() const {
      return h_;
    }

    tasks::task_handle<promise_type> get_handle() const {
      return h_;
    }

    std::shared_future<Ret> get_future() {
      return future_;
    }

    bool await_ready() const noexcept {
      return h_.done();
    }

    template <typename P>
    tasks::task_handle<promise_type> await_suspend(tasks::task_handle<P> h) {
      auto& p = h_.promise();
      p.inherit_from(h.promise());
      p.cont_ = h;
      return h_;
    }

    void await_resume()
      requires(std::same_as<void, Ret>)
    {
      return future_.get();
    }

    Ret await_resume()
      requires((not std::same_as<void, Ret>) and std::is_copy_constructible_v<Ret>)
    {
      return future_.get();
    }

    Ret await_resume()
      requires((not std::same_as<void, Ret>) and (not std::is_copy_constructible_v<Ret>) and std::is_move_constructible_v<Ret>)
    {
      return future_.get();
    }

    void wait() {
      future_.wait();
    }
    std::future_status wait_for(tasks::duration duration_) {
      return future_.wait_for(duration_);
    }
    std::future_status wait_until(tasks::time_point time_point_) {
      return future_.wait_until(time_point_);
    }

    Ret get() {
      return future_.get();
    }
  };

  template <typename>
  struct is_task: std::false_type {};
  template <typename R>
  struct is_task<task<R>>: std::true_type {};

  template <typename T>
  constexpr bool is_task_v = is_task<T>::value;
} // namespace fabric
