// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  pmt.cppm
 *! \brief 
 *!
 */
module;
#include "cyd_fabric/refl/match_type.h"

export module fabric.ts.any;
export import std;

import fabric.logging;

export
class any_base {
protected:
  virtual void* get_value_ptr() = 0;

  [[nodiscard]] virtual const void* get_value_ptr() const = 0;

public:
  virtual ~any_base() = default;

  [[nodiscard]]
  virtual const std::type_info& get_type_info() const = 0;

  template <typename T>
  [[nodiscard]]
  bool is_type() const {
    return get_type_info() == typeid(T);
  }

  template <typename T>
  std::optional<T*> as() {
    if (is_type<T>()) {
      return {static_cast<T*>(get_value_ptr())};
    } else {
      return std::nullopt;
    }
  }

  template <typename T, typename E = std::runtime_error>
  const T& assert_type() const {
    if (!is_type<T>()) {
      LOG::print {ERROR}("any<{}>::assert_type<{}> failed.", get_type_info().name(), typeid(T).name());
      throw E{std::format("any<{}>::assert_type<{}> failed.", get_type_info().name(), typeid(T).name())};
    } else {
      return *static_cast<const T*>(get_value_ptr());
    }
  }

  template <typename T, typename E = std::runtime_error>
  T& assert_type() {
    if (!is_type<T>()) {
      LOG::print {ERROR}("any<{}>::assert_type<{}> failed.", get_type_info().name(), typeid(T).name());
      throw E{std::format("any<{}>::assert_type<{}> failed.", get_type_info().name(), typeid(T).name())};
    } else {
      return *static_cast<T*>(get_value_ptr());
    }
  }
};

export
template <typename T>
class any_impl final: public any_base {
  [[nodiscard]]
  const std::type_info& get_type_info() const override {
    return typeid(T);
  }

  void* get_value_ptr() override {
    return &value_;
  }

  [[nodiscard]]
  const void* get_value_ptr() const override {
    return &value_;
  }
public:
  template <typename... Args>
  explicit any_impl(Args&&... args)
    : value_(std::forward<Args>(args)...)
  { }

  T* operator->() {
    return &value_;
  }

  const T* operator->() const {
    return &value_;
  }

  T& operator*() {
    return &value_;
  }

  const T& operator*() const {
    return &value_;
  }

private:
  T value_;
};

export
using any = std::shared_ptr<any_base>;

export
template <typename T, typename... Args>
any make_any(Args&&... args) {
  return std::make_shared<any_impl<T>>(std::forward<Args>(args)...);
}
