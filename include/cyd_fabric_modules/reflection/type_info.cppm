// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  refl.cppm
 *! \brief
 *!
 */

export module reflect:type_info;

export import std;

export import :types;
export import :type_name;
export import :accessors;

import fabric.logging;

export namespace refl {
  class type_info;

  struct field_info {
    std::size_t index;
    std::string name;
    std::size_t size;
    std::size_t offset;
    access_spec access_type;
    std::function<type_info()> type;

    // Accessors
    void* get_ptr(void* obj) const {
      return (char*)obj + offset;
    }

    template <typename T>
    T& get_ref(void* obj) const {
      return *((T*)get_ptr(obj));
    }
  };

  struct method_info {
    std::size_t index;
    std::string name;
    access_spec access_type;
  };

  class type_info {
  public:
    template <typename T>
    static type_info from() {
      if constexpr (Reflected<T>) {
        static constexpr std::size_t f_count = field_count<T>;
        static constexpr std::size_t m_count = method_count<T>;

        type_info ti{};

        [&]<std::size_t... I>(std::index_sequence<I...>) {
          (
            [&]<typename Field>(Field) {
              static std::size_t off = Field::offset;
              ti.fields_.push_back({
                .index       = Field::index,
                .name        = Field::name,
                .size        = Field::size,
                .offset      = off,
                .access_type = Field::access,
                .type        = []() -> type_info { return from<typename Field::type>(); },
              });
            }(field<T, I>{}),
            ...
          );
        }(std::make_index_sequence<f_count>{});

        [&]<std::size_t... I>(std::index_sequence<I...>) {
          (ti.methods_.push_back({
             // .type        = []() -> type_info { return from<typename field<T, I>::type>(); },
             .index       = I,
             .name        = method<T, I>::name,
             .access_type = method<T, I>::access,
           }),
           ...);
        }(std::make_index_sequence<m_count>{});

        ti.hash_ = typeid(T).hash_code();
        ti.name_ = type_name<T>;
        return ti;
      } else {
        type_info ti{};
        ti.hash_ = typeid(T).hash_code();
        ti.name_ = type_name<T>;
        return ti;
      }
    }

    const std::string& name() const {
      return name_;
    }

    const auto& fields() const {
      return fields_;
    }

    std::size_t hash_code() const {
      return hash_;
    }

    template <typename T>
    bool is_type() const {
      static std::size_t other = typeid(T).hash_code();
      return hash_ == other;
    }
  private:
    std::string name_{};
    std::size_t hash_{};
    std::vector<field_info> fields_{};
    std::vector<method_info> methods_{};
  };
}