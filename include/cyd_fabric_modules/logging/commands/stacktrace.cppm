// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  print.cppm
 *! \brief 
 *!
 */
module;
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <cxxabi.h>

#define NO_COPY(TYPE) TYPE(const TYPE& rhl) = delete; TYPE& operator=(const TYPE& rhl) = delete
#define NO_MOVE(TYPE) TYPE(TYPE&& rhl) = delete; TYPE& operator=(TYPE&& rhl) = delete

export module fabric.logging:print_stacktrace;
import :command_base;

import std;
import std.compat;

export namespace LOG {
  class stacktrace {
    std::string function;
    const char* path;
    std::uint_least32_t linenum;
    LEVEL level;

    std::vector<std::string> backtrace_items{};

  public:
    NO_COPY(stacktrace);

    NO_MOVE(stacktrace);

    explicit constexpr stacktrace(
      const LEVEL level = ERROR,
      unsigned int skip_frames = 0,
      const char* file_name    = normalize(__builtin_FILE(), __FILE__),
      const char* fun          = __builtin_FUNCTION(),
      const unsigned long line = __builtin_LINE()
    ): level(level) {
      function    = fun;
      linenum     = line;
      path        = file_name;

      unw_cursor_t cursor;
      unw_context_t context;

      unw_getcontext(&context);
      unw_init_local(&cursor, &context);

      backtrace_items.emplace_back("Stack backtrace:");
      while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) {
          break;
        }

        if (skip_frames > 0) {
          --skip_frames;
          continue;
        }

        std::stringstream ss;
        ss << std::format("[{}]     at [0x{:X}] ", level.name, pc);
        // std::sprintf("0x%lx:", pc);

        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
          int status;
          char* real_name = abi::__cxa_demangle(sym, nullptr, nullptr, &status);

          if (status == 0) {
            ss << std::format("{} +0x{:X}", std::string {real_name}, offset);
          } else {
            ss << std::format("{} +0x{:X}", std::string {sym}, offset);
          }

          free(real_name);
          // printf(" (%s+0x%lx)\n", sym, offset);
        } else {
          ss << " -- error: unable to obtain symbol name for this frame";
          // printf(" -- error: unable to obtain symbol name for this frame\n");
        }
        backtrace_items.emplace_back(ss.str());
      }
      backtrace_items.emplace_back(std::format("[{}] End stack backtrace. ", level.name));
    }

    ~stacktrace() {
      std::string message{};
      for (const auto & item : backtrace_items) {
        message.append(item).append("\n");
      }
      message = message.substr(0, message.size() - 1);
      log_entry({
        .timestamp = std::chrono::system_clock::now(),
        .path = std::filesystem::path {path},
        .linenum = linenum,
        .function = function,
        .message = message,
        .level = level,
      });
    }
  };
}
