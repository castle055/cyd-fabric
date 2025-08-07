// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:task;
export import :types;
export import :continuation;

import std;
import reflect;

import fabric.logging;


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
      p.cont_ = {p.executor_, h.promise().executor_, h};
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
      requires(
        (not std::same_as<void, Ret>) and (not std::is_copy_constructible_v<Ret>) and
        std::is_move_constructible_v<Ret>
      )
    {
      return future_.get();
    }

    bool done() const {
      return h_.done();
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

  template <typename T>
  concept task_concept = is_task_v<T>;
} // namespace fabric

namespace fabric::tasks {
  export template <typename Ret>
  class task_promise_t: public task_promise_base<Ret> {
  public:
    std::suspend_always initial_suspend() {
      return {};
    }

    continuation_t final_suspend() noexcept {
      return this->cont_;
    }

    void unhandled_exception() {
      this->value_promise_.set_exception(std::current_exception());
    }

    task<Ret> get_return_object() {
      return task<Ret>(
        task_handle<task_promise_t>::from_promise(*this), this->value_promise_.get_future()
      );
    }

    void return_value(Ret value) {
      if constexpr (std::is_lvalue_reference_v<Ret>) {
        this->value_promise_.set_value(value);
      } else if constexpr (std::is_move_constructible_v<Ret>) {
        this->value_promise_.set_value(std::move(value));
      } else {
        this->value_promise_.set_value(value);
      }
    }

    std::suspend_always yield_value(int a) {
      this->reschedule();
      return {};
    }
  };

  export template <typename Ret>
    requires std::is_void_v<Ret>
  class task_promise_t<Ret>: public task_promise_base<Ret> {
  public:
    std::suspend_always initial_suspend() {
      return {};
    }

    continuation_t final_suspend() noexcept {
      return this->cont_;
    }

    void unhandled_exception() {
      this->value_promise_.set_exception(std::current_exception());
    }

    task<> get_return_object() {
      return task<>(
        task_handle<task_promise_t>::from_promise(*this), this->value_promise_.get_future()
      );
    }

    void return_void() {
      this->value_promise_.set_value();
    }
  };
} // namespace fabric::tasks

