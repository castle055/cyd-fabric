// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  keep_alive.cppm
 *! \brief 
 *!
 */

export module fabric.tasks:this_task.keep_alive;
import :task;
import :task_promise;
import :executor;
import :this_task.get_executor;

import std;
import reflect;

export namespace fabric::tasks {
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
} // namespace fabric::tasks


export namespace fabric::this_task {
  task<tasks::keep_alive_token> keep_alive() {
    auto ka = co_await tasks::keep_alive_token::make();
    co_return co_await tasks::keep_alive_token::make();
  }
} // namespace fabric::this_task
