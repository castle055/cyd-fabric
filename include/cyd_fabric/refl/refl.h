// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FABRIC_REFL_H
#define FABRIC_REFL_H

#include <vector>
#include <ranges>

namespace cyd::fabric {
  struct field_base_t {
    virtual ~field_base_t() = default;

    virtual std::size_t get_offset() = 0;

    virtual void* get_value() = 0;

    virtual void set_value(void* new_value) = 0;
  };

  template<typename T>
  struct field_t;

  struct refl_base_t {
    const std::type_info &type_info;
    std::vector<field_base_t*> fields_;
  };

  template<typename T = void>
  class refl: refl_base_t {
    using type_ = T;

  public:
    template<typename FT>
    friend struct field_t;

    refl(): refl_base_t({typeid(T), { }}) {
    }

    // TODO - Are these right?
    template<std::same_as<T> S>
    explicit refl(const refl<S> &rhl): refl_base_t({typeid(T), { }}) {
      for (std::tuple<field_base_t*&, field_base_t*&> field: std::views::zip(this->fields_, rhl.fields_)) {
        std::get<0>(field)->set_value(std::get<1>(field)->get_value());
      }
    }

  public:
    [[nodiscard]] const std::vector<field_base_t*> &fields() const {
      return fields_;
    }

    operator refl<> &() {
      return *reinterpret_cast<refl<>*>(this);
    }
  };

  template<>
  class refl<void>: refl_base_t {
  public:
    refl() = delete;

    ~refl() = delete;

    refl(const refl &rhl) = delete;

    refl(refl &&rhl) = delete;

    refl &operator=(const refl &rhl) = delete;

    refl &operator=(refl &&rhl) = delete;

  public:
    [[nodiscard]] const std::vector<field_base_t*> &fields() const {
      return fields_;
    }

    [[nodiscard]] const std::type_info &get_type() const {
      return type_info;
    }

    template<typename T>
    std::optional<refl<T>*> with_type() & {
      if (type_info == typeid(T)) {
        return reinterpret_cast<refl<T>*>(this);
      } else {
        return std::nullopt;
      }
    }

    template<typename T>
    std::optional<const T*> with_type() const & {
      if (type_info == typeid(T)) {
        return static_cast<const T*>(reinterpret_cast<const refl<T>*>(this));
      } else {
        return std::nullopt;
      }
    }
  };

  template<typename T>
  struct field_t final: field_base_t {
    const std::size_t offset_;
    T value_;

    template<typename P>
    explicit field_t(refl<P>* parent): offset_(
      reinterpret_cast<std::size_t>(this) - reinterpret_cast<std::size_t>(parent)) {
      // const std::size_t size = sizeof(T);
      parent->fields_.push_back(this);
    }

    ~field_t() override = default;

    std::size_t get_offset() override {
      return offset_;
    }

    void* get_value() override {
      return &value_;
    }

    operator T &() {
      return value_;
    }

    void set_value(void* new_value) override {
      value_ = *static_cast<T*>(new_value);
    }

    field_t &operator=(const field_t &rhl) {
      value_ = rhl.value_;
      return *this;
    }

    field_t &operator=(field_t &&rhl) noexcept {
      value_ = std::move(rhl.value_);
      return *this;
    }

    field_t &operator=(const T &new_val) {
      value_ = new_val;
      return *this;
    }

    field_t &operator=(T &&new_val) {
      value_ = std::move(new_val);
      return *this;
    }
  };
}

#endif //FABRIC_REFL_H
