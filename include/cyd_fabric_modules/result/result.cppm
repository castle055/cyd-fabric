// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  result.cppm
 *! \brief
 *!
 */

export module fabric.result;

import std;
import reflect;

import fabric.logging;
export import :result_void;

export namespace fabric {
  template <typename E, typename T = void>
  class result {
    std::variant<std::monostate, T, E> value_{};

    bool is_unset() const {
      return value_.index() == 0;
    }

  public:
    using error_type = E;
    using value_type = T;

    result() = default;
    result(const result& other)
      requires std::is_copy_constructible_v<T>
    {
      if (other.has_value()) {
        value_.template emplace<T>(std::get<1>(other.value_));
      } else {
        if (not other.is_unset()) {
          value_.template emplace<E>(std::get<2>(other.value_));
        }
      }
    }
    result& operator=(const result& other)
      requires std::is_copy_constructible_v<T>
    {
      if (other.has_value()) {
        value_.template emplace<T>(std::get<1>(other.value_));
      } else {
        if (not other.is_unset()) {
          value_.template emplace<E>(std::get<2>(other.value_));
        }
      }
      return *this;
    }
    result(result&& other)
      requires std::is_move_constructible_v<T>
    {
      if (other.has_value()) {
        value_.template emplace<T>(std::move(std::get<1>(other.value_)));
      } else {
        if (not other.is_unset()) {
          value_.template emplace<E>(std::get<2>(other.value_));
        }
      }
    }
    result& operator=(result&& other)
      requires std::is_move_constructible_v<T>
    {
      if (other.has_value()) {
        value_.template emplace<T>(std::move(std::get<1>(other.value_)));
      } else {
        if (not other.is_unset()) {
          value_.template emplace<E>(std::get<2>(other.value_));
        }
      }
      return *this;
    }
    result(const T& val)
        : value_(val) {}
    result(T&& val)
        : value_(std::move(val)) {}
    result(E err)
        : value_(err) {}

    bool has_value() const {
      return value_.index() == 1;
    }
    bool ok() const {
      return has_value();
    }

    T&& value() {
      if (has_value()) {
        return std::move(std::get<1>(value_));
      } else {
        if (is_unset()) {
          throw std::logic_error("result does not hold a value or an error");
        } else {
          throw std::get<2>(value_).make_exception();
        }
      }
    }

    T* operator->() {
      if (has_value()) {
        return &std::get<1>(value_);
      } else {
        if (is_unset()) {
          throw std::logic_error("result does not hold a value or an error");
        } else {
          throw std::get<2>(value_).make_exception();
        }
      }
    }

    E error() const {
      if (not has_value()) {
        if (is_unset()) {
          throw std::logic_error("result does not hold a value or an error");
        } else {
          return std::get<2>(value_);
        }
      } else {
        throw fabric::exception("result does not hold error");
      }
    }

    template <typename U>
      requires(not std::same_as<void, U>)
    result<E, U> map_value(auto&& f) const {
      if (has_value()) {
        return result<E, U>(f(std::move(std::get<1>(value_))));
      } else {
        if (is_unset()) {
          return result<E, U>();
        } else {
          return result<E, U>(std::get<2>(value_));
        }
      }
    }

    template <typename U>
      requires(std::same_as<void, U>)
    result<E> map_value() const {
      if (has_value()) {
        return result<E>();
      } else {
        if (is_unset()) {
          return result<E>();
        } else {
          return result<E>(std::get<2>(value_));
        }
      }
    }

    result<E, T> map_error(auto&& f) const {
      if (has_value()) {
        return result(std::move(std::get<1>(value_)));
      } else {
        if (is_unset()) {
          throw std::logic_error("result does not hold a value or an error");
        } else {
          return result(f(std::get<2>(value_)));
        }
      }
    }

    T&& unwrap(
      const char* file_name    = normalize(__builtin_FILE(), __FILE__),
      const char* fun          = __builtin_FUNCTION(),
      const unsigned long line = __builtin_LINE()
    ) {
      if (has_value()) {
        return std::move(std::get<1>(value_));
      }
      if (is_unset()) {
        throw std::logic_error("result does not hold a value or an error");
      }
      throw std::get<2>(value_).make_exception(0, file_name, fun, line);
    }
  };

  template <typename>
  struct is_result: std::false_type {};
  template <typename E, typename R>
  struct is_result<result<E, R>>: std::true_type {};

  template <typename T>
  constexpr bool is_result_v = is_result<T>::value;
} // namespace fabric::io
