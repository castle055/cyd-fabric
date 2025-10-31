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

namespace fabric {
  export template <typename T>
  class task_result_storage {
    using result_type = std::remove_reference_t<T>;
    alignas(result_type) std::byte storage_[sizeof(result_type)]{};
    bool constructed_{false};

  public:
    task_result_storage() = default;

    ~task_result_storage() {
      if (constructed_) {
        std::destroy_at(reinterpret_cast<result_type*>(&storage_[0]));
      }
    }

    template <typename... Args>
    void construct(Args&&... args) {
      if (constructed_) {
        std::destroy_at(reinterpret_cast<result_type*>(&storage_[0]));
      }
      std::construct_at(reinterpret_cast<result_type*>(&storage_[0]), std::forward<Args>(args)...);
      constructed_ = true;
    }

    T& get() {
      return *reinterpret_cast<result_type*>(&storage_[0]);
    }
    const T& get() const {
      return *reinterpret_cast<result_type*>(&storage_[0]);
    }
  };

  export template <typename T>
  class task_result_storage<T&> {
    T* ptr = nullptr;

  public:
    void construct(T& value) {
      ptr = &value;
    }
    T& get() {
      return *ptr;
    }
    const T& get() const {
      return *ptr;
    }
  };

  export template <typename T>
  class task_result_storage<const T&> {
    const T* ptr = nullptr;

  public:
    void construct(const T& value) {
      ptr = &value;
    }
    const T& get() const {
      return *ptr;
    }
  };

  class task_promise_base {
    std::exception_ptr exception_{nullptr};

  protected:
    bool                                                   detached_{false};
    bool                                                   cancelled_{false};
    std::forward_list<std::pair<tasks::executor*, tasks::task_handle<>>> awaiting_cancellation_{};

  public:
    tasks::executor::sptr                executor_{};
    std::weak_ptr<tasks::schedule_t>     schedule_{};
    std::shared_ptr<tasks::task_context> context_{};
    std::atomic_bool              overridden_ctx_{false};
    tasks::continuation_list_t           cont_{};

    void set_exception(std::exception_ptr exception) {
      exception_ = exception;
    }

    std::exception_ptr& get_exception() {
      return exception_;
    }

    bool is_cancelled() const {
      return cancelled_;
    }

    void await_cancellation(tasks::executor* exec, tasks::task_handle<> h) {
      awaiting_cancellation_.emplace_front(exec, h);
    }

    void detach() {
      detached_ = true;
    }

    void set_executor(const std::shared_ptr<tasks::executor>& e) {
      executor_              = e;
      schedule_              = e->get_schedule();
      context_               = e->get_spawn_context();
      cont_.current_executor = e.get();
    }

    void set_executor(const std::weak_ptr<tasks::executor>& e) {
      auto ex                = e.lock();
      executor_              = ex;
      schedule_              = ex->get_schedule();
      context_               = ex->get_spawn_context();
      cont_.current_executor = ex.get();
    }

    void inherit_from(const auto& parent_task_promise) {
      executor_ = parent_task_promise.executor_;
      schedule_ = parent_task_promise.schedule_;
      if (not overridden_ctx_.load()) {
        context_ = parent_task_promise.context_;
      }
    }

    template <typename T>
    void set_resource(const std::shared_ptr<T>& res, tasks::task_resource_id<T> id = {}) {
      if (not overridden_ctx_.load()) {
        context_ = tasks::task_context::make_copy(context_);
        overridden_ctx_.store(true);
      }
      context_->set_resource(res, id);
    }

    template <typename T>
    bool has_resource(tasks::task_resource_id<T> id = {}) {
      return context_->has_resource(id);
    }

    template <typename T>
    std::shared_ptr<T> get_resource(tasks::task_resource_id<T> id = {}) {
      return context_->get_resource(id);
    }

    void reschedule() {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue(
          tasks::task_handle<task_promise_base>::from_promise(*static_cast<task_promise_base*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    void reschedule(tasks::time_point when) {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue_delayed(
          when, tasks::task_handle<task_promise_base>::from_promise(*static_cast<task_promise_base*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    void reschedule(tasks::duration delay) {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue_delayed(
          tasks::clock::now() + delay,
          tasks::task_handle<task_promise_base>::from_promise(*static_cast<task_promise_base*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    std::weak_ptr<tasks::executor> get_executor() const {
      return executor_;
    }

    void cancel() {
      if (not cancelled_) {
        cancelled_ = true;
        for (const auto& [exec, h]: awaiting_cancellation_) {
          exec->schedule_handle(h);
        }
        awaiting_cancellation_.clear();
      }
    }
  };
} // namespace fabric::tasks

namespace fabric {
  export template <typename Ret>
  class task_promise_t: public task_promise_base {
    task_result_storage<Ret> return_value_{};

  public:
    auto& get_result() /*requires (not std::is_rvalue_reference_v<Ret>)*/ {
      return return_value_.get();
    }

    std::suspend_always initial_suspend() {
      return {};
    }

    tasks::continuation_list_t final_suspend() noexcept {
      this->cont_.current_executor = this->executor_.get();
      this->cont_.detached         = this->detached_;
      return this->cont_;
    }

    void unhandled_exception() {
      this->set_exception(std::current_exception());
    }

    task<Ret> get_return_object() {
      return task<Ret>{tasks::task_handle<task_promise_t>::from_promise(*this)};
    }

    void return_value(Ret value) {
      if constexpr (std::is_lvalue_reference_v<Ret>) {
        return_value_.construct(value);
      } else if constexpr (std::is_move_constructible_v<Ret>) {
        return_value_.construct(std::move(value));
      } else {
        return_value_.construct(value);
      }
    }

    // std::suspend_always yield_value(int a) {
    //   this->reschedule();
    //   return {};
    // }
  };

  export template <typename Ret>
    requires std::is_void_v<Ret>
  class task_promise_t<Ret>: public task_promise_base {
  public:
    std::suspend_always initial_suspend() {
      return {};
    }

    tasks::continuation_list_t final_suspend() noexcept {
      this->cont_.current_executor = this->executor_.get();
      this->cont_.detached         = this->detached_;
      return this->cont_;
    }

    void unhandled_exception() {
      this->set_exception(std::current_exception());
    }

    task<> get_return_object() {
      return task<>{tasks::task_handle<task_promise_t>::from_promise(*this)};
    }

    void return_void() {}

    // std::suspend_always yield_value(int a) {
    //   this->reschedule();
    //   return {};
    // }
  };
} // namespace fabric::tasks
