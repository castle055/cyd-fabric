// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VARIABLES_H
#define VARIABLES_H

#include <hocon/config_mergeable.hpp>

#include <map>

namespace cyd::fabric::config {
  struct variables_t: std::map<std::string, std::string> {
    variables_t(): map({
      {"ASD", "BYE"},
      {"BYE<>", "bye"},
    }) {
    }
  };

  inline void asdfasdf() {
    auto c = hocon::config::parse_string( R"(
host: wow
)");
  }

  // struct variables_t final: ucl::Ucl::variable_replacer {
  //   [[nodiscard]] bool is_variable(const std::string &str) const override {
  //     std::cout << "is_variable(\"" << str << "\")" << std::endl;
  //     return !str.empty();
  //   }
  //
  //   [[nodiscard]] std::string replace(const std::string &var) const override {
  //     if (var == "ASD") {
  //       std::cout << "replace(\"" << var << "\")" << std::endl;
  //       return "hello";
  //     } else {
  //       return "";
  //     }
  //   }
  // };
}

#endif //VARIABLES_H
