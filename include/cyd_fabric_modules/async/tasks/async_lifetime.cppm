// Copyright (c) 2025-2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  async_lifetime.cppm
 *! \brief
 *!
 */

module;
#include <fabric/unwrap.h>

export module fabric.tasks:async_lifetime;

import std;
import reflect;

export import fabric.result;
export import :task_promise;
export import :this_task.keep_alive;

namespace fabric::tasks {
  template <typename Data>
  struct async_lifetime_data: public Data {
    tasks::executor::sptr owner_executor;
    keep_alive_token      ka_token; // keep executor alive until request is done

    template <typename... Args>
    async_lifetime_data(
      const tasks::executor::sptr& owner_executor, keep_alive_token&& ka_token, Args&&... args
    )
        : Data(std::forward<Args>(args)...),
          owner_executor(owner_executor),
          ka_token(std::move(ka_token)) {}
  };
} // namespace fabric::tasks

export namespace fabric::tasks {
  template <typename T, typename Data>
  class async_lifetime {
  protected:
    std::unique_ptr<async_lifetime_data<Data>> data{nullptr};

  private:
    template <typename S, typename... Args>
    using ConstructRet =
      typename decltype(std::declval<S>().constructor(std::declval<Args>()...))::return_type;

    template <typename S>
    using DestructRet = typename decltype(std::declval<S>().destructor(*data))::return_type;

  public:
    async_lifetime() = default;

    ~async_lifetime() {
      if (nullptr != data) {
        data->owner_executor
          ->schedule([](std::unique_ptr<async_lifetime_data<Data>> data) -> task<> {
            if (ResultConcept<DestructRet<T>>) {
              (co_await T::destructor(*data)).throw_error();
              co_return;
            } else {
              co_await T::destructor(*data);
              co_return;
            }
          }(std::move(data)))
          .detach();
      }
    }

    template <typename S = T>
    task<DestructRet<S>> self_destruct() {
      if (data == nullptr) {
        throw std::logic_error("async_lifetime::self_destruct: already destroyed or moved!");
      }
      if constexpr (std::is_void_v<DestructRet<T>>) {
        co_await T::destructor(*data);
        data = nullptr;
        co_return;
      } else {
        auto r = co_await T::destructor(*data);
        data   = nullptr;
        co_return std::move(r);
      }
    }

    async_lifetime(const async_lifetime& other)            = delete;
    async_lifetime& operator=(const async_lifetime& other) = delete;
    async_lifetime(async_lifetime&& other) noexcept {
      data       = std::move(other.data);
      other.data = nullptr;
    }
    async_lifetime& operator=(async_lifetime&& other) noexcept {
      data       = std::move(other.data);
      other.data = nullptr;
      return *this;
    }

    template <typename S = T, typename... Args>
      requires(std::same_as<S, T>)
    static task<map_result_type<ConstructRet<S, Args...>, S>> make(Args&&... args) {
      T obj{};
      auto res = co_await obj.constructor(std::forward<Args>(args)...);
      if (not res.ok()) {
        co_return res.error();
      }
      co_return std::move(obj);
    }

  protected:
    template <typename... Args>
    task<> init_data(Args&&... args) {
      if (nullptr != data) {
        throw std::logic_error{"async_lifetime: already initialized"};
      }
      auto exec     = co_await this_task::get_executor_sptr();
      auto ka_token = co_await this_task::keep_alive();
      data          = std::make_unique<async_lifetime_data<Data>>(
        exec, std::move(ka_token), std::forward<Args>(args)...
      );
      co_return;
    }
  };

} // namespace fabric::tasks
