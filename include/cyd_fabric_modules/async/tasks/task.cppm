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
    using return_type  = Ret;

  private:
    tasks::task_handle<promise_type> h_;

  public:
    task(tasks::task_handle<promise_type> h__)
        : h_(h__) {}


    operator tasks::task_handle<promise_type>() const {
      return h_;
    }

    tasks::task_handle<promise_type> get_handle() const {
      return h_;
    }

    bool running() const {
      auto& p = h_.promise();
      return nullptr != p.executor_;
    }

    bool await_ready() const noexcept {
      return h_.done();
    }

    template <typename P>
    tasks::task_handle<> await_suspend(tasks::task_handle<P> h) {
      if (done()) {
        return h;
      }
      auto& p = h_.promise();
      p.cont_.emplace_continuation(h.promise().executor_.get(), h);
      if (not running()) {
        p.inherit_from(h.promise());
        return h_;
      } else {
        return std::noop_coroutine();
      }
    }

    auto await_resume()
      requires(
        (not std::same_as<void, Ret>) and std::is_rvalue_reference_v<Ret> and
        (std::is_copy_constructible_v<Ret>)
      )
    {
      if constexpr (not std::is_void_v<Ret>) {
        auto e = h_.promise().get_exception();
        if (e != nullptr) {
          throw e;
        }
        std::remove_reference_t<Ret>& res{h_.promise().get_result()};
        return std::move(res);
      }
    }

    auto& await_resume()
      requires(
        (not std::same_as<void, Ret>) and std::is_rvalue_reference_v<Ret> and
        (not std::is_copy_constructible_v<Ret>)
      )
    {
      if constexpr (not std::is_void_v<Ret>) {
        auto e = h_.promise().get_exception();
        if (e != nullptr) {
          throw e;
        }
        std::remove_reference_t<Ret>& res{h_.promise().get_result()};
        return res;
      }
    }

    auto& await_resume()
      requires((not std::same_as<void, Ret>) and std::is_lvalue_reference_v<Ret>)
    {
      if constexpr (not std::is_void_v<Ret>) {
        auto e = h_.promise().get_exception();
        if (e != nullptr) {
          throw e;
        }
        Ret res{h_.promise().get_result()};
        return res;
      }
    }

    auto await_resume()
      requires(
        (not std::same_as<void, Ret>) and (not std::is_reference_v<Ret>) and
        (std::is_copy_constructible_v<Ret>)
      )
    {
      if constexpr (not std::is_void_v<Ret>) {
        auto e = h_.promise().get_exception();
        if (e != nullptr) {
          throw e;
        }
        Ret res{h_.promise().get_result()};
        // h_.destroy();
        return std::move(res);
      }
    }

    void await_resume() &
      requires(
        (std::is_void_v<Ret>) or
        ((not std::is_reference_v<Ret>) and (not std::is_copy_constructible_v<Ret>))
      )
    {
      auto e = h_.promise().get_exception();
      if (e != nullptr) {
        throw e;
      }
    }

    Ret await_resume() &&
      requires(
        (not std::same_as<void, Ret>) and (not std::is_reference_v<Ret>) and
        (not std::is_copy_constructible_v<Ret>)
      )
    {
      // if constexpr (not std::is_void_v<Ret>) {
      auto e = h_.promise().get_exception();
      if (e != nullptr) {
        throw e;
      }
      Ret res = std::move(h_.promise().get_result());
      // h_.destroy();
      return std::move(res);
      // }
    }

    task operator co_await() & {
      return *this;
    }

    struct immediate_awaiter {
      task task;

      bool await_ready() const noexcept {
        return task.done();
      }
      template <typename P>
      tasks::task_handle<> await_suspend(tasks::task_handle<P> h) {
        return task.await_suspend(h);
      }
      void await_resume()
        requires(std::same_as<void, Ret>)
      {}
      auto& await_resume()
        requires((not std::same_as<void, Ret>) and (std::is_reference_v<Ret>))
      {
        return task.await_resume();
      }
      auto await_resume()
        requires(
          (not std::same_as<void, Ret>) and (not std::is_reference_v<Ret>) and
          (std::is_copy_constructible_v<Ret>)
        )
      {
        return task.await_resume();
      }
      auto await_resume()
        requires(
          (not std::same_as<void, Ret>) and (not std::is_reference_v<Ret>) and
          (not std::is_copy_constructible_v<Ret>) and (std::is_move_constructible_v<Ret>)
        )
      {
        return std::move(std::move(task).await_resume());
      }
    };
    immediate_awaiter operator co_await() && {
      return immediate_awaiter{*this};
    }

    bool done() const {
      return h_.done();
    }
    // void wait() {
    //   h_.promise().get_completion_latch().wait();
    // }
    // void try_wait() {
    //   h_.promise().get_completion_latch().try_wait();
    // }

    auto& get()
      requires((not std::same_as<void, Ret>) and std::is_copy_constructible_v<Ret>)
    {
      if constexpr (not std::is_void_v<Ret>) {
        return h_.promise().get_result();
      }
    }

    auto&& get()
      requires(
        (not std::same_as<void, Ret>) and (not std::is_copy_constructible_v<Ret>) and
        std::is_move_constructible_v<Ret>
      )
    {
      if constexpr (not std::is_void_v<Ret>) {
        return std::move(h_.promise().get_result());
      }
    }

    void cancel() {
      if (not done()) {
        h_.promise().cancel();
      }
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
