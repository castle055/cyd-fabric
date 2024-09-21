// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  pretty_print.cppm
 *! \brief
 *!
 */

export module reflect:equality;

import std;

export import fabric.ts.packs;
export import fabric.logging;

export import :types;
export import :accessors;

export namespace refl {
  template <Reflected R>
  bool deep_eq(const R& lhs, const R& rhs);
}

namespace refl::deep_eq_impl {
  template <Reflected R, typename field_data>
  bool field_eq(const R& lhs, const R& rhs) {
    const auto& field1 = field_data::from_instance(lhs);
    const auto& field2 = field_data::from_instance(rhs);

    if constexpr (field_data::is_reference) {
      if (&field1 == &field2) {
        return true;
      }

      using field_type = std::remove_reference_t<typename field_data::type>;
      if constexpr (std::equality_comparable<field_type>) {
        return field1 == field2;
      } else if constexpr (Reflected<field_type>) {
        return deep_eq(field1, field2);
      }
    } else if constexpr (field_data::is_pointer) {
      if (field1 == field2) {
        return true;
      }

      using field_type = std::remove_pointer_t<typename field_data::type>;
      if constexpr (std::equality_comparable<field_type>) {
        return *field1 == *field2;
      } else if constexpr (Reflected<field_type>) {
        return deep_eq(*field1, *field2);
      }
    } else {
      if constexpr (std::equality_comparable<typename field_data::type>) {
        return field1 == field2;
      } else if constexpr (Reflected<typename field_data::type>) {
        return deep_eq(field1, field2);
      }
    }

    return false;
  }
} // namespace refl::deep_eq_impl

namespace refl {
  template <Reflected R>
  bool deep_eq(const R& lhs, const R& rhs) {
    static constexpr auto count = field_count<R>;
    return for_each_field<R, deep_eq_impl::field_eq>(lhs, rhs);
  }
}