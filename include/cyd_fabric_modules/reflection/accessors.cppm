// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  accessors.cppm
 *! \brief
 *!
 */

export module reflect:accessors;

import std;

export import fabric.ts.packs;

export import :types;
export import :type_name;

template <typename Type, std::size_t Offset>
struct representation {
  unsigned char offset_[Offset];
  Type          value;
};

template <typename Type>
struct representation<Type, 0> {
  Type          value;
};


export namespace refl {
  using namespace fabric::ts::packs;

  enum class field_access : unsigned char {
    NONE      = 0U,
    PRIVATE   = 1U,
    PROTECTED = 2U,
    PUBLIC    = 3U,
  };

  template <refl::Reflected T, std::size_t I>
  struct field {
    static constexpr const char* name    = T::__type_info__::field_names[I];
    using type                           = get<I, typename T::__type_info__::field_types>::type;
    static constexpr std::size_t  size   = get<I, typename T::__type_info__::field_sizes>::value;
    static constexpr std::size_t  offset = get<I, typename T::__type_info__::field_offsets>::value;
    static constexpr field_access access =
      field_access{get<I, typename T::__type_info__::field_access_specifiers>::value};
    static constexpr bool is_reference = std::is_reference_v<type>;

    static const std::remove_reference_t<type>& from_instance(const T& instance) {
      const auto* rep = reinterpret_cast<const representation<type, offset>*>(&instance);
      return rep->value;
    }
  };

  template <refl::Reflected T>
  constexpr std::size_t field_count = get_size<typename T::__type_info__::field_types>::value;
}