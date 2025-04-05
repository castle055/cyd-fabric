// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  error.cppm
 *! \brief
 *!
 */

export module fabric.io.error;

import std;
import reflect;

import fabric.logging;
import fabric.exception;

export namespace fabric {
  class io_exception final: public exception_base<io_exception> {
  public:
    explicit constexpr io_exception(
      const std::string& message,
      const unsigned int skip_frames = 0,
      const char* file_name          = normalize(__builtin_FILE(), __FILE__),
      const char* fun                = __builtin_FUNCTION(),
      const unsigned long line       = __builtin_LINE()
    )
        : exception_base(message, skip_frames + 1, file_name, fun, line) {}
  };
} // namespace fabric

export namespace fabric::io {
  struct error {
    int         error_code;
    std::string message;

    static constexpr error from_code(int code) {
      return {code, std::strerror(code)};
    }

    std::string to_string() const {
      return std::format(" ERROR ({}) {}", error_code, message);
    }

    void print() const {
      LOG::print{ERROR}("IO Error ({}) {}", error_code, message);
    }

    [[noreturn]]
    void throw_exception(
      const unsigned int skip_frames = 0,
      const char* file_name          = normalize(__builtin_FILE(), __FILE__),
      const char* fun                = __builtin_FUNCTION(),
      const unsigned long line       = __builtin_LINE()
    ) const {
      throw io_exception(message, 1 + skip_frames, file_name, fun, line);
    }

    io_exception make_exception(
      const unsigned int skip_frames = 0,
      const char* file_name          = normalize(__builtin_FILE(), __FILE__),
      const char* fun                = __builtin_FUNCTION(),
      const unsigned long line       = __builtin_LINE()
    ) const {
      return io_exception(message, 1 + skip_frames, file_name, fun, line);
    }
  };
} // namespace fabric::io
