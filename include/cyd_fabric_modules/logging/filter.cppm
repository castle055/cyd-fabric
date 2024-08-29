// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  filter.cppm
 *! \brief 
 *!
 */

export module fabric.logging:filter;

export import fabric.logging.levels;
import fabric.logging.target_base;

export namespace LOG {
  struct integer_range_t: public std::pair<unsigned int, unsigned int> {
    explicit integer_range_t(unsigned int value)
      : std::pair<unsigned int, unsigned int>(value, value) {
    }

    integer_range_t(unsigned int from, unsigned int to)
      : std::pair<unsigned int, unsigned int>(from, to) {
    }

    integer_range_t(const LEVEL& value)
      : std::pair<unsigned int, unsigned int>(value.priority, value.priority) {
    }

    integer_range_t(const LEVEL& from, const LEVEL& to)
      : std::pair<unsigned int, unsigned int>(from.priority, to.priority) {
    }

    bool contains(unsigned int value) const {
      return value >= this->first && value <= this->second;
    }
  };

  struct integer_constraint_t {
    std::vector<integer_range_t> ranges{};

    explicit integer_constraint_t(const std::initializer_list<integer_range_t> ranges_) {
      for (const auto & range : ranges_) {
        ranges.emplace_back(range);
      }
    }

    bool matches(const std::size_t num) const {
      for (const auto & range : ranges) {
        if (range.contains(num)) {
          return true;
        }
      }
      return false;
    }

  };

  class filter_constraint_t {
  public:
    filter_constraint_t() = default;

    void set_path_regex(const std::regex &regex) {
      path_regex_ = regex;
      has_path_   = true;
    }

    void set_function_regex(const std::regex &regex) {
      function_regex_ = regex;
      has_function_   = true;
    }

    void set_message_regex(const std::regex &regex) {
      message_regex_ = regex;
      has_message_   = true;
    }

    void set_linenum_constraint(const integer_constraint_t &constraint) {
      linenum_constraint_ = constraint;
      has_linenum_        = true;
    }

    void set_level_constraint(const integer_constraint_t &constraint) {
      level_constraint_ = constraint;
      has_level_        = true;
    }

    void set_path_regex(std::regex &&regex) {
      path_regex_ = regex;
      has_path_   = true;
    }

    void set_function_regex(std::regex &&regex) {
      function_regex_ = regex;
      has_function_   = true;
    }

    void set_message_regex(std::regex &&regex) {
      message_regex_ = regex;
      has_message_   = true;
    }

    void set_linenum_constraint(integer_constraint_t &&constraint) {
      linenum_constraint_ = constraint;
      has_linenum_        = true;
    }

    void set_level_constraint(integer_constraint_t &&constraint) {
      level_constraint_ = constraint;
      has_level_        = true;
    }

    bool matches_path(const std::string &path) const {
      if (!has_path_) return true;
      return std::regex_match(path, path_regex_);
    }
    bool matches_path(const std::filesystem::path &path) const {
      if (!has_path_) return true;
      return std::regex_match(path.string(), path_regex_);
    }
    bool matches_function(const std::string &function) const {
      if (!has_function_) return true;
      return std::regex_match(function, function_regex_);
    }
    bool matches_function(const std::string_view &function) const {
      if (!has_function_) return true;
      return std::regex_match(function.begin(), function.end(), function_regex_);
    }
    bool matches_message(const std::string &message) const {
      if (!has_message_) return true;
      return std::regex_match(message, message_regex_);
    }
    bool matches_message(const std::string_view &message) const {
      if (!has_message_) return true;
      return std::regex_match(message.begin(), message.end(), message_regex_);
    }
    bool matches_linenum(const unsigned int linenum) const {
      if (!has_linenum_) return true;
      return linenum_constraint_.matches(linenum);
    }
    bool matches_level(const unsigned int level) const {
      if (!has_level_) return true;
      return level_constraint_.matches(level);
    }
    bool matches_level(const LEVEL& level) const {
      if (!has_level_) return true;
      return level_constraint_.matches(level.priority);
    }
  private:
    bool has_path_      {false};
    bool has_function_  {false};
    bool has_message_   {false};
    bool has_linenum_   {false};
    bool has_level_     {false};
    std::regex path_regex_ { };
    std::regex function_regex_ { };
    std::regex message_regex_ { };
    integer_constraint_t linenum_constraint_ { };
    integer_constraint_t level_constraint_ { };
  };

  struct filter_t {
    filter_constraint_t constraint_{};
    target_id target_ {"default"};

    filter_t(const char* path, const char* target)
      : target_(target)
    {
      constraint_.set_path_regex(std::regex {path});
    };
    filter_t(const filter_constraint_t &constraint, const target_id& target)
      : constraint_(constraint), target_(target)
    {
    };
  };

  template <typename ReturnContext>
  class filter_builder_t {
  public:
    explicit filter_builder_t(ReturnContext &return_ctx_ref)
      : return_ctx_ref_(return_ctx_ref) {
    };

    filter_builder_t &path(const std::string &string) {
      constraint_.set_path_regex(std::regex {string});
      return *this;
    }

    filter_builder_t &path(std::string &&string) {
      constraint_.set_path_regex(std::regex {string});
      return *this;
    }

    filter_builder_t &function(const std::string &string) {
      constraint_.set_function_regex(std::regex {string});
      return *this;
    }

    filter_builder_t &function(std::string &&string) {
      constraint_.set_function_regex(std::regex {string});
      return *this;
    }

    filter_builder_t &message(const std::string &string) {
      constraint_.set_message_regex(std::regex {string});
      return *this;
    }

    filter_builder_t &message(std::string &&string) {
      constraint_.set_message_regex(std::regex {string});
      return *this;
    }

    filter_builder_t &path(const std::regex &regex) {
      constraint_.set_path_regex(regex);
      return *this;
    }

    filter_builder_t &path(std::regex &&regex) {
      constraint_.set_path_regex(std::forward<std::regex &&>(regex));
      return *this;
    }

    filter_builder_t &function(const std::regex &regex) {
      constraint_.set_function_regex(regex);
      return *this;
    }
    filter_builder_t& function(std::regex&& regex) {
      constraint_.set_function_regex(std::forward<std::regex&&>(regex));
      return *this;
    }
    filter_builder_t& message(const std::regex &regex) {
      constraint_.set_message_regex(regex);
      return *this;
    }
    filter_builder_t& message(std::regex&& regex) {
      constraint_.set_message_regex(std::forward<std::regex&&>(regex));
      return *this;
    }
    filter_builder_t& linenum(const integer_constraint_t &constraint) {
      constraint_.set_linenum_constraint(constraint);
      return *this;
    }
    filter_builder_t& linenum(integer_constraint_t&& constraint) {
      constraint_.set_linenum_constraint(std::forward<integer_constraint_t&&>(constraint));
      return *this;
    }

    filter_builder_t& levels(const std::initializer_list<integer_range_t> range_) {
      constraint_.set_level_constraint(integer_constraint_t {range_});
      return *this;
    }

    ReturnContext& operator[](const std::string& target_name) {
      target_ = target_id {target_name};
      return return_ctx_ref_;
    }
    ReturnContext& operator[](std::string&& target_name) {
      target_ = target_id {target_name};
      return return_ctx_ref_;
    }


    filter_t build() const {
      return filter_t {constraint_, target_};
    }
  private:
    filter_constraint_t constraint_{};
    target_id target_{"default"};

    ReturnContext& return_ctx_ref_;
  };
}
