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

namespace fabric::tasks {
  export template <typename Ret>
  class task_promise_base {
  public:
    executor::sptr                executor_{};
    std::weak_ptr<schedule_t>     schedule_{};
    std::shared_ptr<task_context> context_{};
    std::atomic_bool              overridden_ctx_{false};
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
      if (not overridden_ctx_.load()) {
        context_ = parent_task_promise.context_;
      }
    }

    template <typename T>
    void set_resource(const std::shared_ptr<T>& res, task_resource_id<T> id = {}) {
      if (not overridden_ctx_.load()) {
        context_ = task_context::make_copy(context_);
        overridden_ctx_.store(true);
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

    void reschedule() {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue(
          task_handle<task_promise_base>::from_promise(*static_cast<task_promise_base*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    void reschedule(time_point when) {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue_delayed(
          when, task_handle<task_promise_base>::from_promise(*static_cast<task_promise_base*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    void reschedule(duration delay) {
      if (not schedule_.expired()) {
        schedule_.lock()->enqueue_delayed(
          clock::now() + delay,
          task_handle<task_promise_base>::from_promise(*static_cast<task_promise_base*>(this))
        );
      } else {
        throw std::logic_error{"invalid schedule"};
      }
    }

    std::weak_ptr<executor> get_executor() const {
      return executor_;
    }
  };
} // namespace fabric::tasks
