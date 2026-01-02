// Copyright (c) 2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  SourceLocation.cppm
 *! \brief
 *!
 */

export module fabric.source_location;

import std;
import reflect;

constexpr const char* normalize(const char* path, const char* path_ref) {
  std::size_t i = 0;
  while (path[i] == path_ref[i] && path[i] != '\0' && path_ref[i] != '\0') {
    ++i;
  }
  return path + i;
}

namespace fabric {
  export struct SourceLocation {
    const char*         file_name;
    const char*         function_name;
    const unsigned long line;
    const unsigned long column;

    constexpr SourceLocation(
      const char*         file_name = normalize(__builtin_FILE(), __FILE__),
      const char*         fun       = __builtin_FUNCTION(),
      const unsigned long line      = __builtin_LINE(),
      const unsigned long column    = __builtin_COLUMN()
    )
        : file_name(file_name),
          function_name(fun),
          line(line),
          column(column) {}

    std::string to_string() const {
      const std::string fun_name{function_name};
      if (fun_name.empty()) {
        return std::format("{}:{}:{}:static_init", file_name, line, column);
      } else if (fun_name.ends_with("()")) {
        return std::format("{}:{}:{}:{}", file_name, line, column, function_name);
      }
      return std::format("{}:{}:{}:{}()", file_name, line, column, function_name);
    }
  };
} // namespace fabric
