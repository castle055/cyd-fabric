// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  _types.cppm
 *! \brief
 *!
 */

export module reflect:types;

export import fabric.ts.packs;

export namespace refl {
  template <typename T>
  concept is_type_info = requires {
    typename T::field_types;
    typename T::field_sizes;
    typename T::field_offsets;
  };

  template <typename T>
  concept Reflected = requires {
    typename T::__type_info__;
    is_type_info<typename T::__type_info__>;
  };
} // namespace refl

export 
{
  template <typename... T>
  using refl_pack = fabric::ts::packs::pack<T...>;
  template <std::size_t ...T>
  using refl_int_pack = fabric::ts::packs::integer_pack<T...>;
}
