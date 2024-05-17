// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  logging.cppm
 *! \brief 
 *!
 */

module;
#include <chrono>
#include <format>
#include <iostream>
#include <stacktrace>
#include <filesystem>
#include <regex>
#include <source_location>
#include <unordered_map>
#include <memory>
#include <string>

#define NO_COPY(TYPE) TYPE(const TYPE& rhl) = delete; TYPE& operator=(const TYPE& rhl) = delete
#define NO_MOVE(TYPE) TYPE(TYPE&& rhl) = delete; TYPE& operator=(TYPE&& rhl) = delete

export module fabric.logging;
export import fabric.logging.target_base;

export import :log_levels;
// export import :log_entry;

template<>
struct std::formatter<LOG::entry_t> {
private:
  static constexpr format_parse_context::iterator advance(
    format_parse_context::iterator it
  ) {
    return (++it);
  }

  constexpr bool parse_field_fmt(
    format_parse_context::iterator &first,
    format_parse_context::iterator last,
    const std::string &field_name
  ) const {
    auto start = first;

    std::size_t i = 0;
    while (first != last && i < field_name.size()) {
      if (*first != field_name[i]) {
        first = start;
        return false;
      }
      ++first;
      ++i;
    }

    return true;
  }

public:
  constexpr format_parse_context::iterator parse(
    format_parse_context &parse_ctx
  ) {
    if (parse_ctx.begin() == parse_ctx.end() || *parse_ctx.begin() == '}') {
      return parse_ctx.begin();
    }
    // parse_ctx.next_arg_id();
    auto last  = parse_ctx.end();
    auto first = parse_ctx.begin();
    // std::cout << "[BEGIN PARSE]" << std::endl;
    // for (auto &it: parse_ctx) {
    //   std::cout << it << std::endl;
    // }
    // std::cout << "[END PARSE]" << std::endl;
    if (parse_field_fmt(first, last, "timestamp")) {
      entry_field = entry_field_e::TIMESTAMP;
      return (first);
    } else if (parse_field_fmt(first, last, "path")) {
      entry_field = entry_field_e::PATH;
      return (first);
    } else if (parse_field_fmt(first, last, "line")) {
      entry_field = entry_field_e::LINENUM;
      return (first);
    } else if (parse_field_fmt(first, last, "function")) {
      entry_field = entry_field_e::FUNCTION;
      return (first);
    } else if (parse_field_fmt(first, last, "message")) {
      entry_field = entry_field_e::MESSAGE;
      return (first);
    } else {
      throw std::format_error {"format error: invalid format-spec for log entry"};
      return (last);
    }
  }

  format_context::iterator format(
    LOG::entry_t entry,
    format_context &fmt_ctx
  ) const {
    std::ostringstream ss { };
    switch (entry_field) {
      case entry_field_e::NONE:
        break;
      case entry_field_e::TIMESTAMP:
        ss << std::format("{0:%Y-%m-%d} {0:%H:%M:%S}", entry.timestamp);
        break;
      case entry_field_e::PATH:
        // ss << std::format("{}", entry.path);
        ss << entry.path.string();
        break;
      case entry_field_e::LINENUM:
        ss << std::format("{}", entry.linenum);
        break;
      case entry_field_e::FUNCTION:
        ss << std::format("{}", entry.function);
        break;
      case entry_field_e::MESSAGE:
        ss << entry.message;
        break;
    }
    for (const char &c: ss.view()) {
      *fmt_ctx.out()++ = c;
    }
    return fmt_ctx.out();
  }

private:
  enum class entry_field_e {
    NONE,
    TIMESTAMP,
    PATH,
    LINENUM,
    FUNCTION,
    MESSAGE
  } entry_field = entry_field_e::NONE;
};

export namespace LOG {
  struct entry_format_t {
    explicit entry_format_t(const std::string &fmt)
      : format_(std::regex_replace(fmt, std::regex {"\\{entry:"}, "{0:")) {
    }

    [[nodiscard]]
    std::string format(const entry_t &entry) const {
      return std::vformat(std::string_view {format_}, std::make_format_args(entry));
    }

  private:
    std::string format_;
  };
}

export namespace LOG { namespace TARGETS {
    class STDOUT final: public target_base {
      explicit STDOUT(
        entry_format_t entry_format
      ): target_base("stdout"), entry_format_(std::move(entry_format)) {
      }

    public:
      ~STDOUT() noexcept override = default;

      void append(const entry_t &entry) override {
        std::cout << entry_format_.format(entry) << std::endl;
      }

    private:
      entry_format_t entry_format_ {"{entry:message}"};

    public: //! Builder
      struct BUILDER {
        BUILDER() = default;

        explicit BUILDER(std::string &&format): entry_format_(format) {
        }

        BUILDER &entry_format(std::string &&fmt) {
          entry_format_ = entry_format_t {fmt};
          return *this;
        }

        [[nodiscard]] std::unique_ptr<target_base> operator()() const {
          return std::unique_ptr<STDOUT>(new STDOUT(entry_format_));
        }

      private:
        entry_format_t entry_format_ {"{entry:message}"};
      };
    };

    class STDERR final: public target_base {
      explicit STDERR(
        entry_format_t entry_format
      ): target_base("stderr"), entry_format_(std::move(entry_format)) {
      }

    public:
      ~STDERR() noexcept override = default;

      void append(const entry_t &entry) override {
        std::cerr << entry_format_.format(entry) << std::endl;
      }

    private:
      entry_format_t entry_format_ {"{entry:message}"};

    public: //! Builder
      struct BUILDER {
        BUILDER() = default;

        explicit BUILDER(std::string &&format): entry_format_(format) {
        }

        BUILDER &entry_format(std::string &&fmt) {
          entry_format_ = entry_format_t {fmt};
          return *this;
        }

        [[nodiscard]] std::unique_ptr<target_base> operator()() const {
          return std::unique_ptr<STDERR>(new STDERR(entry_format_));
        }

      private:
        entry_format_t entry_format_ {"{entry:message}"};
      };
    };

    class FILE: public target_base {
      FILE(
        std::string id,
        entry_format_t entry_format
      ): target_base(std::move(id)), entry_format_(std::move(entry_format)) {
        std::cout << "FILE: CONSTRUCTOR" << std::endl;
      }

    public:
      ~FILE() noexcept override {
        std::cout << "FILE: DESTRUCTOR" << std::endl;
      }

      void append(const entry_t &entry) override {
        std::cout << entry_format_.format(entry) << std::endl;
      }

    private:
      entry_format_t entry_format_ {"{entry:message}"};

    public: //! Builder
      struct BUILDER {
        explicit BUILDER(std::string &&id): id_(std::move(id)) {
        }

        BUILDER &entry_format(std::string &&fmt) {
          entry_format_ = entry_format_t {fmt};
          return *this;
        }

        [[nodiscard]] std::unique_ptr<target_base> operator()() const {
          return std::unique_ptr<FILE>(new FILE(id_, entry_format_));
        }

      private:
        std::string id_;
        entry_format_t entry_format_ {"{entry:message}"};
      };
    };
  }

  //
}


export namespace LOG {
  struct filter_t {
    std::regex pattern_;
    target_id target_ {"default"};

    filter_t(const char*&&pattern, const char*&&target)
      : pattern_(pattern), target_(target) {
    }
  };
}

//! CONFIGURATION
namespace LOG::config {
  std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> default_targets() {
    std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> ts { };
    ts.emplace(target_id {"stdout"}.id_hash, std::move(TARGETS::STDOUT::BUILDER { }()));
    ts.emplace(target_id {"stderr"}.id_hash, std::move(TARGETS::STDERR::BUILDER { }()));
    return ts;
  }

  std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> targets = default_targets();
  std::vector<filter_t> filters { };
}

export namespace LOG {
  struct INIT {
    explicit INIT(const char* inilizer_file = __builtin_FILE()) {
    }

    NO_COPY(INIT);

    NO_MOVE(INIT);

    ~INIT() {
      config::targets.swap(targets_);
      config::filters.swap(filters_);
    }

  public:
    INIT &FILTER(filter_t &&filter) {
      if (!targets_.contains(filter.target_.id_hash)) {
        throw std::runtime_error {std::format("Target ID \"{}\" does not exists", filter.target_.id_string)};
      }
      filters_.emplace_back(filter);
      return *this;
    }

    INIT &FILTERS(std::initializer_list<filter_t> &&filters) {
      for (auto &&filter: filters) {
        if (!targets_.contains(filter.target_.id_hash)) {
          throw std::runtime_error {std::format("Target ID \"{}\" does not exists", filter.target_.id_string)};
        }
        filters_.emplace_back(filter);
      }
      return *this;
    }

  private:
    void add_target(std::unique_ptr<target_base> &&target) {
      if (targets_.contains(target->id.id_hash)) {
        throw std::runtime_error {std::format("Target ID \"{}\" has already been defined", target->id.id_string)};
      } else {
        targets_.emplace(target->id.id_hash, std::move(target));
      }
    }

  public:
    INIT &TARGET(const TargetBuilder auto &builder) {
      // using target_type = std::remove_reference_t<decltype(target)>;
      add_target(std::move(builder()));
      return *this;
    }

    template<TargetBuilder... TBs>
    INIT &TARGETS(TBs... builders) {
      // using target_type = std::remove_reference_t<decltype(target)>;
      (add_target(std::move(builders())), ...);
      return *this;
    }

  private:
    std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> targets_ = config::default_targets();
    std::vector<filter_t> filters_ { };
  };

  void log_entry(const entry_t &entry) {
    for (const auto &filter: config::filters) {
      if (std::regex_match(entry.path.string(), filter.pattern_)) {
        config::targets.at(filter.target_.id_hash)->append(entry);
      }
    }
  }
}

consteval const char* normalize(const char* path) {
  std::size_t i           = 0;
  constexpr const char* p = __builtin_FILE();
  while (p[i] == path[i] && path[i] != '\0' && p[i] != '\0') {
    ++i;
  }
  return path + i;
}

export namespace LOG {
  class print {
    std::string message;
    std::string function;
    std::filesystem::path path;
    std::uint_least32_t linenum;

  public:
    NO_COPY(print);

    NO_MOVE(print);

    explicit constexpr print(
      const LEVEL level,
      const char* file_name    = normalize(__builtin_FILE()),
      const char* fun          = __builtin_FUNCTION(),
      const unsigned long line = __builtin_LINE()
    ) {
      function = fun;
      linenum  = line;
      path     = std::filesystem::path {file_name};
    }

    ~print() {
      log_entry({
        .timestamp = std::chrono::system_clock::now(),
        .path = path,
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
