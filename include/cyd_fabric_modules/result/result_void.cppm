// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  result.cppm
 *! \brief
 *!
 */

export module fabric.result:result_void;

import std;
import reflect;

import fabric.logging;
import fabric.exception;

export namespace fabric {
  template <typename E, typename T = void>
  class result;

  template <typename E>
  class result<E, void> {
    std::optional<E> error_;

  public:
    using error_type = E;
    using value_type = void;

    result(const result& other)
        : error_(other.error_) {}
    result& operator=(const result& other) {
      error_ = other.error_;
      return *this;
    }
    result()
        : error_(std::nullopt) {}
    result(E err)
        : error_(err) {}

    bool has_value() const {
      return not error_.has_value();
    }
    bool ok() const {
      return has_value();
    }

    E error() const {
      if (not has_value()) {
        return error_.value();
      } else {
        throw fabric::exception("result does not hold error");
      }
    }

    template <typename U>
      requires(not std::same_as<void, U>)
    result<E, U> map_value(auto&& f) const {
      if (has_value()) {
        return result<E, U>(f());
      } else {
        return result<E, U>(error_.value());
      }
    }

    template <typename U>
      requires std::same_as<void, U>
    result map_value() const {
      if (has_value()) {
        return result();
      } else {
        return result(error_.value());
      }
    }

    result map_error(auto&& f) const {
      if (has_value()) {
        return result();
      } else {
        return result(f(error_.value()));
      }
    }

    void unwrap(
      const char* file_name    = normalize(__builtin_FILE(), __FILE__),
      const char* fun          = __builtin_FUNCTION(),
      const unsigned long line = __builtin_LINE()
    ) const {
      if (not has_value()) {
        throw error_.value().make_exception(0, file_name, fun, line);
      }
    }
  };
} // namespace fabric::io
