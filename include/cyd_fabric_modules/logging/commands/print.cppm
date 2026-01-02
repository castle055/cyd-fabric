// Copyright (c) 2024-2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  print.cppm
 *! \brief
 *!
 */
module;
#define NO_COPY(TYPE)                                                                              \
  TYPE(const TYPE& rhl)            = delete;                                                       \
  TYPE& operator=(const TYPE& rhl) = delete
#define NO_MOVE(TYPE)                                                                              \
  TYPE(TYPE&& rhl)            = delete;                                                            \
  TYPE& operator=(TYPE&& rhl) = delete

export module fabric.logging:print;
export import :command_base;

import std;

export namespace LOG {
  class print {
    std::string            message;
    fabric::SourceLocation location;
    LEVEL                  level;

  public:
    NO_COPY(print);

    NO_MOVE(print);

    explicit constexpr print(const LEVEL level, const fabric::SourceLocation& location = {})
        : location(location),
          level(level) {}

    ~print() {}

    template <typename... Args>
    constexpr void operator()(const std::format_string<Args...> fmt, Args&&... args) {
      message = std::format(fmt, std::forward<Args&&>(args)...);

      log_entry({
        .timestamp = std::chrono::system_clock::now(),
        .path      = std::filesystem::path{location.file_name},
        .linenum   = location.line,
        .function  = location.function_name,
        .message   = message,
        .level     = level,
      });
    }
  };
} // namespace LOG
