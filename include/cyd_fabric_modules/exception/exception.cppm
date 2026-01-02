// Copyright (c) 2025-2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  exception.cppm
 *! \brief
 *!
 */

export module fabric.exception;

import std;
import reflect;

import fabric.logging;
import fabric.source_location;

export namespace fabric {
  class exception_base: public std::runtime_error {
  public:
    SourceLocation source_location;

    explicit constexpr exception_base(
      const std::string_view name,
      const std::string&     message,
      const unsigned int     skip_frames     = 0,
      const SourceLocation&  source_location = {}
    )
        : std::runtime_error(message),
          source_location(source_location) {
      LOG::print{ERROR, source_location}("Exception ({}): {}", name, message);
      LOG::stacktrace{ERROR, 1 + skip_frames, source_location};
    }
  };

  class exception final: public exception_base {
  public:
    explicit constexpr exception(
      const std::string&    message,
      const unsigned int    skip_frames     = 0,
      const SourceLocation& source_location = {}
    )
        : exception_base("fabric::exception", message, skip_frames + 1, source_location) {}
  };
} // namespace fabric
