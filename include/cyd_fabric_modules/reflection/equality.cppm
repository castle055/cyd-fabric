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
  template <typename I>
  bool std_iterable_eq(const I& lhs, const I& rhs) {
    using T = typename I::value_type;

    if (lhs.size() != rhs.size()) {
      return false;
    }
    if constexpr (std::equality_comparable<T>) {
      for (auto it1 = lhs.begin(), it2 = rhs.begin(); it1 != lhs.end() && it2 != rhs.end(); ++it1, ++it2) {
        if (*it1 != *it2) {
          return false;
        }
      }
      return true;
    } else if constexpr (Reflected<T>) {
      for (auto it1 = lhs.begin(), it2 = rhs.begin(); it1 != lhs.end() && it2 != rhs.end(); ++it1, ++it2) {
        if (deep_eq(*it1, *it2)) {
          return false;
        }
      }
      return true;
    }
  }

  template <typename T>
  bool ref_eq(const T& lhs, const T& rhs) {
    using fabric::ts::packs::is_type;

    if constexpr (is_type<std::vector, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::list, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::deque, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::queue, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::stack, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::map, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::unordered_map, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::set, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (is_type<std::unordered_set, T>::value) {
      return std_iterable_eq(lhs, rhs);
    } else if constexpr (std::equality_comparable<T>) {
      return lhs == rhs;
    } else if constexpr (Reflected<T>) {
      return deep_eq(lhs, rhs);
    }
  }

  template <Reflected R, typename field_data>
  bool field_eq(const R& lhs, const R& rhs) {
    const auto& field1 = field_data::from_instance(lhs);
    const auto& field2 = field_data::from_instance(rhs);

    if constexpr (field_data::is_reference) {
      if (&field1 == &field2) {
        return true;
      }

      using field_type = std::remove_reference_t<typename field_data::type>;
      return ref_eq<field_type>(field1, field2);
    } else if constexpr (field_data::is_pointer) {
      if (field1 == field2) {
        return true;
      }

      using field_type = std::remove_pointer_t<typename field_data::type>;
      return ref_eq<field_type>(*field1, *field2);
    } else {
      using field_type = typename field_data::type;
      return ref_eq<field_type>(field1, field2);
    }

    return false;
  }
} // namespace refl::deep_eq_impl

namespace refl {
  template <Reflected R>
  bool deep_eq(const R& lhs, const R& rhs) {
    static constexpr auto count = field_count<R>;

    auto impl = [&]<std::size_t... I>(std::index_sequence<I...>) {
      return ((deep_eq_impl::field_eq<R, field<R, I>>(lhs, rhs)) && ...);
    };

    return impl(std::make_index_sequence<count>{});
  }
}