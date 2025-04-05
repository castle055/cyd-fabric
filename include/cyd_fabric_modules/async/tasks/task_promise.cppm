// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:task_promise;

import std;
import reflect;

import fabric.logging;
import fabric.concurrent_queue;

export import :types;
export import :task;
export import :schedule;
export import :continuation;
export import :executor;

using namespace std::chrono_literals;

export namespace fabric::tasks {
  template <typename Ret>
  struct task_promise_base {
    executor::sptr                executor_{};
    std::weak_ptr<schedule_t>     schedule_{};
    std::shared_ptr<task_context> context_{};
    std::atomic_bool              overriden_ctx_{false};
    std::promise<Ret>             value_promise_{};
    std::optional<task_handle<>>  cont_{std::nullopt};

    void set_executor(const std::shared_ptr<executor>& e) {
      executor_ = e;
      schedule_ = e->get_schedule();
      context_  = e->get_spawn_context();
    }

    void set_executor(const std::weak_ptr<executor>& e) {
      auto ex   = e.lock();
      executor_ = ex;
      schedule_ = ex->get_schedule();
      context_  = ex->get_spawn_context();
    }

    void inherit_from(const auto& parent_task_promise) {
      executor_ = parent_task_promise.executor_;
      schedule_ = parent_task_promise.schedule_;
      if (not overriden_ctx_.load()) {
        context_ = parent_task_promise.context_;
      }
    }

    template <typename T>
    void set_resource(const std::shared_ptr<T>& res, task_resource_id<T> id = {}) {
      if (not overriden_ctx_.load()) {
        context_ = task_context::make_copy(context_);
        overriden_ctx_.store(true);
      }
      context_->set_resource(res, id);
    }

    template <typename T>
    bool has_resource(task_resource_id<T> id = {}) {
      return context_->has_resource(id);
    }

    template <typename T>
    std::shared_ptr<T> get_resource(task_resource_id<T> id = {}) {
      return context_->get_resource(id);
    }

    std::suspend_always initial_suspend() {
      return {};
    }
    continuation_t final_suspend() noexcept {
      return {cont_};
    }
    void unhandled_exception() {
      value_promise_.set_exception(std::current_exception());
    }

    // std::suspend_always yield_value(Ret a) {
    //   // value_promise_.set_value(value);
    //   return {};
    // }

    // std::suspend_never yield_value(int a) {
    //   // value_promise_.set_value(value);
    //   return {};
    // }

    void reschedule() {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue(
          task_handle<task_promise_t<Ret>>::from_promise(*static_cast<task_promise_t<Ret>*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    void reschedule(time_point when) {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue_delayed(
          when,
          task_handle<task_promise_t<Ret>>::from_promise(*static_cast<task_promise_t<Ret>*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    void reschedule(duration delay) {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue_delayed(
          clock::now() + delay,
          task_handle<task_promise_t<Ret>>::from_promise(*static_cast<task_promise_t<Ret>*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    std::weak_ptr<executor> get_executor() const {
      return executor_;
    }
  };

  template <typename Ret>
  struct task_promise_t: task_promise_base<Ret> {
    task<Ret> get_return_object() {
      return task<Ret>(
        task_handle<task_promise_t>::from_promise(*this), this->value_promise_.get_future()
      );
    }
    void return_value(Ret&& value) {
      this->value_promise_.set_value(std::move(value));
    }
  };

  template <>
  struct task_promise_t<void>: task_promise_base<void> {
    task<> get_return_object() {
      return task<>(
        task_handle<task_promise_t>::from_promise(*this), this->value_promise_.get_future()
      );
    }
    void return_void() {
      value_promise_.set_value();
    }
  };

} // namespace fabric::tasks

  struct awaitable_get_executor {
    std::weak_ptr<fabric::tasks::executor> val;

    bool await_ready() const noexcept {
      return false;
    }
    template <typename P>
    bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
      val = h.promise().get_executor();
      return false;
    }
    std::weak_ptr<fabric::tasks::executor> await_resume() const noexcept {
      return val;
    }
  };

  template <typename ResourceType>
  struct awaitable_get_resource {
    std::shared_ptr<ResourceType>         val;
    fabric::tasks::task_resource_id<ResourceType> id;

    explicit awaitable_get_resource(fabric::tasks::task_resource_id<ResourceType> id_ = {})
        : id(id_) {}

    bool await_ready() const noexcept {
      return false;
    }
    template <typename P>
    bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
      val = h.promise().get_resource(id);
      return false;
    }
    std::shared_ptr<ResourceType> await_resume() const noexcept {
      return val;
    }
  };

  template <typename ResourceType>
  struct awaitable_set_resource {
    std::shared_ptr<ResourceType>         val;
    fabric::tasks::task_resource_id<ResourceType> id;

    explicit awaitable_set_resource(
      const std::shared_ptr<ResourceType>& ptr_, fabric::tasks::task_resource_id<ResourceType> id_ = {}
    )
        : val(ptr_),
          id(id_) {}

    bool await_ready() const noexcept {
      return false;
    }
    template <typename P>
    bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
      h.promise().set_resource(val, id);
      return false;
    }
    void await_resume() const noexcept {}
  };

  template <typename ResourceType>
  struct awaitable_has_resource {
    bool                                  val;
    fabric::tasks::task_resource_id<ResourceType> id;

    explicit awaitable_has_resource(fabric::tasks::task_resource_id<ResourceType> id_ = {})
        : id(id_) {}

    bool await_ready() const noexcept {
      return false;
    }
    template <typename P>
    bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
      val = h.promise().has_resource(id);
      return false;
    }
    bool await_resume() const noexcept {
      return val;
    }
  };

  struct awaitable_sleep {
    fabric::tasks::duration       delay;
    fabric::tasks::executor::sptr exec;

    bool await_ready() const noexcept {
      return delay == std::chrono::nanoseconds::zero();
    } /// Always suspend!

    template <typename P>
    void await_suspend(fabric::tasks::task_handle<P> h) noexcept {
      exec = h.promise().get_executor().lock();
      h.promise().reschedule(delay);
    }

    void await_resume() const noexcept {
    }
  };

export namespace fabric::this_task {
  task<const tasks::executor&> get_executor() {
    co_return *(co_await awaitable_get_executor{}).lock();
  }

  task<tasks::executor::sptr> get_executor_sptr() {
    co_return (co_await awaitable_get_executor{}).lock();
  }

  template <typename ResourceType>
  task<std::shared_ptr<ResourceType>> get_resource(tasks::task_resource_id<ResourceType> id = {}) {
    co_return co_await awaitable_get_resource<ResourceType>{id};
  }

  template <typename ResourceType>
  awaitable_set_resource<ResourceType> set_resource(
    const std::shared_ptr<ResourceType>& ptr, tasks::task_resource_id<ResourceType> id = {}
  ) {
    //! Cannot be a task<> because then we would be overriding the resource for
    //! this task<> and not the parent, which is the one who wants the override.
    return awaitable_set_resource<ResourceType>{ptr, id};
  }

  template <typename ResourceType>
  task<bool> has_resource(tasks::task_resource_id<ResourceType> id = {}) {
    co_return co_await awaitable_has_resource<ResourceType>{id};
  }

  task<> sleep(auto duration) {
    co_await awaitable_sleep{duration};
  }

  class keep_alive_token {
    tasks::executor::sptr owner_executor_;
    bool                  moved_ = false;

    keep_alive_token(const tasks::executor::sptr& exec)
        : owner_executor_(exec) {}

  public:
    keep_alive_token(const keep_alive_token& other)            = delete;
    keep_alive_token& operator=(const keep_alive_token& other) = delete;
    keep_alive_token(keep_alive_token&& other) noexcept {
      owner_executor_ = other.owner_executor_;
      moved_          = other.moved_;
      other.moved_    = true;
    }
    keep_alive_token& operator=(keep_alive_token&& other) noexcept {
      owner_executor_ = other.owner_executor_;
      moved_          = other.moved_;
      other.moved_    = true;
      return *this;
    }

    static task<keep_alive_token> make() {
      auto exec = co_await this_task::get_executor_sptr();
      exec->keep_alive(true);
      co_return {exec};
    }

    ~keep_alive_token() {
      if (not moved_) {
        owner_executor_->keep_alive(false);
      }
    }
  };

  task<keep_alive_token> keep_alive() {
    co_return std::move(co_await keep_alive_token::make());
  }
} // namespace fabric::this_task

export awaitable_sleep operator co_await(fabric::tasks::duration delay) {
  return awaitable_sleep{delay};
}

export awaitable_sleep operator co_await(fabric::tasks::time_point time_point) {
  auto now = fabric::tasks::clock::now();
  auto dur = time_point - now;
  if (dur >= decltype(dur)::zero()) {
    return awaitable_sleep{dur};
  }
  return awaitable_sleep{0ms};
}
