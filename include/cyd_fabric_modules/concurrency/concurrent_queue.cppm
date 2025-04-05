// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  concurrent_queue.cppm
 *! \brief
 *!
 */

export module fabric.concurrent_queue;

import std;
import reflect;

export namespace fabric {
  template <typename T, template <typename> class Container = std::queue>
  class concurrent_queue {
    mutable std::shared_mutex mtx_{};
    Container<T>              data_{};

  public:
    concurrent_queue()                                   = default;
    concurrent_queue(const concurrent_queue&)            = delete;
    concurrent_queue& operator=(const concurrent_queue&) = delete;

    bool empty() const {
      std::shared_lock lk{mtx_};
      return data_.empty();
    }

    void clear() {
      std::unique_lock lk{mtx_};
      data_.clear();
    }

    template <std::convertible_to<T> U>
    void push(U&& value) {
      std::unique_lock lk{mtx_};
      data_.push(std::forward<U>(value));
    }

    [[nodiscard]]
    std::optional<T> try_pop() {
      std::unique_lock lk{mtx_};
      if (data_.empty()) {
        return std::nullopt;
      }
      if constexpr (std::same_as<Container<T>, std::priority_queue<T>>) {
        T val = std::move_if_noexcept(data_.top());
        data_.pop();
        return val;
      } else {
        T val = std::move_if_noexcept(data_.front());
        data_.pop();
        return val;
      }
    }

    [[nodiscard]]
    bool try_pop(T& value) {
      std::unique_lock lk{mtx_};
      if (data_.empty()) {
        return false;
      }
      if constexpr (std::same_as<Container<T>, std::priority_queue<T>>) {
        value = std::move_if_noexcept(data_.top());
      } else {
        value = std::move_if_noexcept(data_.front());
      }
      data_.pop();
      return true;
    }
  };
} // namespace fabric
