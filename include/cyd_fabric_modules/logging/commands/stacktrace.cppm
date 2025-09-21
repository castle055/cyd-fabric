// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  print.cppm
 *! \brief
 *!
 */
module;
#define UNW_LOCAL_ONLY
#include <cxxabi.h>
#include <libunwind.h>

#define NO_COPY(TYPE)                                                                              \
  TYPE(const TYPE& rhl)            = delete;                                                       \
  TYPE& operator=(const TYPE& rhl) = delete
#define NO_MOVE(TYPE)                                                                              \
  TYPE(TYPE&& rhl)            = delete;                                                            \
  TYPE& operator=(TYPE&& rhl) = delete

export module fabric.logging:print_stacktrace;
import :command_base;

import std;
import std.compat;
import reflect;

import fabric.logging.ansi_codes;
import fabric.thread_name;

static const std::vector<std::regex> thread_noise_patterns = {
  std::regex{"^void std::__invoke.*"},
  std::regex{"^std::__invoke.*"},
  std::regex{"^void std::thread::_Invoker.*"},
  std::regex{"^std::thread::_Invoker.*"},
  std::regex{"^std::thread::_State.*"},
  std::regex{"^execute_native_thread_routine"},
  std::regex{"^start_thread"},
  std::regex{"^clone.*"},
};

static const std::vector<std::regex> tasks_noise_patterns = {
  std::regex{"^std::coroutine_handle<.*>::resume.*"},
  std::regex{"^std::\\w+::coroutine_handle<.*>::resume.*"},
  std::regex{"^fabric::tasks::schedule_t@fabric.tasks::run.*"},
  std::regex{"^fabric::tasks::schedule_t@fabric.tasks::run_all.*"},
  std::regex{"^fabric::tasks::executor_thread_t@fabric.tasks::executor_thread_t.*"},
};

static bool matches_noise(const std::vector<std::regex>& patterns, const std::string& str) {
  for (const auto& pattern: patterns) {
    if (std::regex_match(str, pattern)) {
      return true;
    }
  }
  return false;
}

static std::string
replace_all(const std::string& str, const std::string& from, const std::string& to) {
  std::size_t       i{0}, pos{0};
  std::stringstream ss{};
  while ((pos = str.find(from, i)) != std::string::npos) {
    ss << str.substr(i, pos - i);
    ss << to;
    i = pos + from.length();
  }
  if (i < str.length()) {
    ss << str.substr(i);
  }
  return ss.str();
}

static std::string cleanup_name(const std::string& str) {
  std::string s{str};
  if (str.ends_with("[clone .resume]")) {
    s = std::format(
      "{}[async]{} {}",
      fabric::ansi::color::yellow_bri,
      fabric::ansi::reset,
      str.substr(0, str.size() - 16)
    );
  }
  s = std::regex_replace(
    s,
    std::regex{"\\$_(\\d+)::operator\\(\\)"},
    std::format("{}[lambda_$1]{}", fabric::ansi::color::blue, fabric::ansi::reset)
  );
  s = std::regex_replace(s, std::regex{"@[\\w.]+"}, "");
  s = std::regex_replace(s, std::regex{"std::__n4861"}, "std");
  s = std::regex_replace(s, std::regex{"std::__1"}, "std");
  s = std::regex_replace(
    s,
    std::regex{"(([\\w]+::)+)"},
    std::format("{}$1{}", fabric::ansi::color::black_bri, fabric::ansi::reset)
  );
  s = std::regex_replace(
    s,
    std::regex{"([()])"},
    std::format("{}$1{}", fabric::ansi::color::blue_bri, fabric::ansi::reset)
  );
  return s;
}

export namespace LOG {
  class stacktrace {
    std::string         function;
    const char*         path;
    std::uint_least32_t linenum;
    LEVEL               level;

    std::vector<std::string> backtrace_items{};

  public:
    NO_COPY(stacktrace);

    NO_MOVE(stacktrace);

    explicit constexpr stacktrace(
      const LEVEL         level       = ERROR,
      unsigned int        skip_frames = 0,
      const char*         file_name   = normalize(__builtin_FILE(), __FILE__),
      const char*         fun         = __builtin_FUNCTION(),
      const unsigned long line        = __builtin_LINE()
    )
        : level(level) {
      function = fun;
      linenum  = line;
      path     = file_name;

      unw_cursor_t  cursor;
      unw_context_t context;

      unw_getcontext(&context);
      unw_init_local(&cursor, &context);

      std::size_t frame_index              = 0;
      int         thread_startup_machinery = 0;
      int         tasks_machinery          = 0;
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
        // ss << std::format("[{}] #{:<3} ", level.name, (frame_index++));
        ss << fabric::ansi::color::black_bri;
        ss << std::format("#{:<3} ", (frame_index++));
        ss << fabric::ansi::reset;
        // std::sprintf("0x%lx:", pc);

        char sym[1024];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
          int   status;
          char* real_name = abi::__cxa_demangle(sym, nullptr, nullptr, &status);

          if (status == 0) {
            std::string rname{real_name};
            if (matches_noise(thread_noise_patterns, rname)) {
              if (thread_startup_machinery++ == 0) {
                ss << fabric::ansi::color::black_bri;
                ss << "...thread startup...";
                ss << fabric::ansi::reset;
              } else {
                continue;
              }
            } else if (matches_noise(tasks_noise_patterns, rname)) {
              if (tasks_machinery++ == 0) {
                ss << fabric::ansi::color::yellow_bri;
                ss << std::format("[executor: {}]", fabric::get_thread_name());
                ss << fabric::ansi::reset;
              } else {
                continue;
              }
            } else {
              thread_startup_machinery = 0;
              tasks_machinery          = 0;
              ss << std::format("{}", cleanup_name(rname));
            }
          } else {
            std::string rname{sym};
            if (matches_noise(thread_noise_patterns, rname)) {
              if (thread_startup_machinery++ == 0) {
                ss << fabric::ansi::color::black_bri;
                ss << "...thread startup...";
                ss << fabric::ansi::reset;
              } else {
                continue;
              }
            } else if (matches_noise(tasks_noise_patterns, rname)) {
              if (tasks_machinery++ == 0) {
                ss << fabric::ansi::color::yellow_bri;
                ss << std::format("[executor: {}]", fabric::get_thread_name());
                ss << fabric::ansi::reset;
              } else {
                continue;
              }
            } else {
              thread_startup_machinery = 0;
              tasks_machinery          = 0;
              ss << std::format("[0x{:X}+{:X}] {}", pc, offset, cleanup_name(rname));
            }
          }

          free(real_name);
          // printf(" (%s+0x%lx)\n", sym, offset);
        } else {
          ss << fabric::ansi::color::black_bri;
          ss << std::format("[0x{:X}] <unknown>", pc);
          ss << fabric::ansi::reset;
          // printf(" -- error: unable to obtain symbol name for this frame\n");
        }
        backtrace_items.emplace_back(ss.str());
      }
    }

    ~stacktrace() {
      std::string message{};
      for (const auto& item: backtrace_items) {
        message.append(item).append("\n");
      }
      message = message.substr(0, message.size() - 1);
      log_entry({
        .timestamp = std::chrono::system_clock::now(),
        .path      = std::filesystem::path{path},
        .linenum   = linenum,
        .function  = function,
        .message   = message,
        .level     = level,
      });
    }
  };
} // namespace LOG
