// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:detached_task;
export import :types;
export import :task;
export import :task_promise;

import std;
import reflect;

import fabric.logging;
import fabric.exception;


using namespace std::chrono_literals;

export namespace fabric {
  class detached_task {
    tasks::task_handle<> h_;

    task_promise_base& get_promise() const {
      return std::coroutine_handle<task_promise_base>::from_address(h_.address()).promise();
    }

  public:
    explicit detached_task(const tasks::task_handle<> h__)
        : h_(h__) {
      get_promise().detach();
    }

    bool running() const {
      return nullptr != h_ and nullptr != get_promise().executor_;
    }

    bool done() const {
      return h_.done();
    }

    bool await_ready() const noexcept {
      return done();
    }

    template <typename P>
    tasks::task_handle<> await_suspend(tasks::task_handle<P> h) {
      if (done()) {
        return h;
      }
      auto& p = get_promise();
      p.cont_.emplace_continuation(h.promise().executor_.get(), h);
      return std::noop_coroutine();
    }

    void await_resume() {
      auto e = get_promise().get_exception();
      if (e != nullptr) {
        throw e;
      }
    }

    void cancel() {
      if (not done()) {
        get_promise().cancel();
      } else {
        LOG::print{WARN}("Task already completed");
      }
    }
  };

  template <typename Ret>
  detached_task task<Ret>::detach() {
    if (not running()) {
      throw fabric::exception{"Attempted to detach a non-running task"};
    }
    detached_task dtask{h_};
    this->h_ = nullptr;
    return dtask;
  }
} // namespace fabric
