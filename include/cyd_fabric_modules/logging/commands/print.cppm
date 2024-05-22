// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  print.cppm
 *! \brief 
 *!
 */
module;
#define NO_COPY(TYPE) TYPE(const TYPE& rhl) = delete; TYPE& operator=(const TYPE& rhl) = delete
#define NO_MOVE(TYPE) TYPE(TYPE&& rhl) = delete; TYPE& operator=(TYPE&& rhl) = delete

export module fabric.logging:print;
import :command_base;

constexpr const char* normalize(const char* path, const char* path_ref) {
  std::size_t i = 0;
  while (path[i] == path_ref[i] && path[i] != '\0' && path_ref[i] != '\0') {
    ++i;
  }
  return path + i;
}

export namespace LOG {
  class print {
    std::string message;
    std::string function;
    const char* path;
    std::uint_least32_t linenum;

  public:
    NO_COPY(print);

    NO_MOVE(print);

    explicit constexpr print(
      const LEVEL level,
      const char* file_name    = normalize(__builtin_FILE(), __FILE__),
      const char* fun          = __builtin_FUNCTION(),
      const unsigned long line = __builtin_LINE()
    ) {
      function = fun;
      linenum  = line;
      path     = file_name;
    }

    ~print() {
      log_entry({
        .timestamp = std::chrono::system_clock::now(),
        .path = std::filesystem::path {path},
        .linenum = linenum,
        .function = function,
        .message = message,
      });
    }

    template<typename... Args>
    constexpr void operator()(
      const std::format_string<Args...> fmt,
      Args &&... args
    ) { message = std::format(fmt, std::forward<Args &&>(args)...); }
  };
}
