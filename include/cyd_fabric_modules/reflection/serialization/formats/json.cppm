// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  pretty_print.cppm
 *! \brief
 *!
 */

module;
#include <nlohmann/json.hpp>

export module reflect.marshal.formats.json;

import std;

import packtl;
import reflect;

import reflect.marshal.formats.base;

using JSON = nlohmann::json;

export namespace formats {
  template <typename O>
  struct json_fmt: refl::visitor<json_fmt<O>> {
    struct args_t {
      bool         pretty = true;
      unsigned int indent = 2;
    };

    explicit json_fmt(O& out_, args_t args_)
        : refl::visitor<json_fmt<O>>(),
          out(out_),
          args(args_) {}

    template <typename T>
    void handle_value(const T& it) {
      if constexpr (std::is_same_v<T, std::atomic_flag>) {
        current() = (it.test() ? "SET" : "CLEAR");
      } else if constexpr (packtl::is_type<std::unique_ptr, T>::value) {
        const auto* value = it.get();
        out << " {";
        out << std::format("0x{:X}", (std::size_t)value);
        out << "} ";
      } else if constexpr (packtl::is_type<std::shared_ptr, T>::value) {
        const auto* value = it.get();
        out << "{";
        out << std::format("0x{:X}", (std::size_t)value);
        out << "} ";
      } else if constexpr (packtl::is_type<std::weak_ptr, T>::value) {
        const auto* value = it.get();
        out << " {";
        out << std::format("0x{:X}", (std::size_t)value);
        out << "}: ";
      } else if constexpr (refl::Reflected<T>) {
        if (visited_.contains((std::size_t)&it)) {
          out << "<circular reference>";
          return;
        }
        visited_.emplace((std::size_t)&it);
      } else if constexpr (std::is_convertible_v<T, std::string>) {
        current() = std::format("{}", it);
      } else if constexpr (std::same_as<T, char>) {
        current() = std::format("0x{:X} '{}'", static_cast<int>(it), it);
      } else if constexpr (std::formattable<T, char>) {
        current() = std::format("{}", it);
      }

      this->visit_value(it);
    }

    template <typename T>
    void handle_iterable(const T& iterable) {
      this->visit_iterable(iterable);
    }

    template <typename T, typename Field>
    void handle_field(const T& obj) {
      current()[Field::name] = JSON{};
      push(current()[Field::name]);

      this->template visit_obj_field<T, Field>(obj);

      pop();
    }

    template <typename T>
    void handle_obj(const T& obj) {
      // current()["__type"] = refl::type_name<T>;
      this->visit_obj(obj);
    }

    template <refl::Reflected R>
    void serialize(const R& obj) {
      json_         = JSON{};
      push(json_);
      this->visit(obj);
      out << json_.dump(2);
    }

  private:
    JSON& current() {
      return *obj_stack.top();
    }

    void push(JSON& obj) {
      obj_stack.push(&obj);
    }

    void pop() {
      obj_stack.pop();
    }
  private:
    std::unordered_set<std::size_t> visited_{};
    O&                              out;
    args_t                          args;
    JSON                            json_;
    std::stack<JSON*>               obj_stack;
  };
} // namespace formats
