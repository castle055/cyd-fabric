// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  exception.cppm
 *! \brief 
 *!
 */

export module fabric.exception;

import std;
import reflect;

import fabric.logging;

export namespace fabric {
  template <typename ExceptionType>
  class exception_base: public std::runtime_error {
  public:
    explicit constexpr exception_base(
      const std::string& message,
      const unsigned int skip_frames = 0,
      const char* file_name          = normalize(__builtin_FILE(), __FILE__),
      const char* fun                = __builtin_FUNCTION(),
      const unsigned long line       = __builtin_LINE()
    )
        : std::runtime_error(message) {
      LOG::print{ERROR, file_name, fun, line}("Exception ({}): {}", refl::type_name<ExceptionType>, message);
      LOG::stacktrace{ERROR, 1 + skip_frames, file_name, fun, line};
    }
  };

  class exception final: public exception_base<exception> {
  public:
    explicit constexpr exception(
      const std::string& message,
      const unsigned int skip_frames = 0,
      const char* file_name          = normalize(__builtin_FILE(), __FILE__),
      const char* fun                = __builtin_FUNCTION(),
      const unsigned long line       = __builtin_LINE()
    )
        : exception_base(message, skip_frames + 1, file_name, fun, line) {}
  };
}