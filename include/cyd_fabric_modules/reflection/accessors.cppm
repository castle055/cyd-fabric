// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  accessors.cppm
 *! \brief
 *!
 */

export module reflect:accessors;

import std;

import packtl;

import :types;
import :type_name;

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

  enum class field_access : unsigned char {
    NONE      = 0U,
    PRIVATE   = 1U,
    PROTECTED = 2U,
    PUBLIC    = 3U,
  };

  template <refl::Reflected T, std::size_t I>
  struct field {
    static constexpr std::size_t index   = I;
    static constexpr const char* name    = T::__type_info__::field_names[I];
    using type                           = typename packtl::get<I, typename T::__type_info__::field_types>::type;
    static constexpr std::size_t  size   = packtl::get<I, typename T::__type_info__::field_sizes>::value;
    static constexpr std::size_t  offset = packtl::get<I, typename T::__type_info__::field_offsets>::value;
    static constexpr field_access access =
      field_access{packtl::get<I, typename T::__type_info__::field_access_specifiers>::value};

    static constexpr bool is_reference = std::is_reference_v<type>;
    static constexpr bool is_pointer   = std::is_pointer_v<type>;

    static const std::remove_reference_t<type>& from_instance(const T& instance) {
      const auto* rep = reinterpret_cast<const representation<type, offset>*>(&instance);
      return rep->value;
    }
  };

  template <refl::Reflected T>
  constexpr std::size_t field_count = packtl::get_size<typename T::__type_info__::field_types>::value;

  template <refl::Reflected T, std::size_t I>
  struct method {
    static constexpr std::size_t index   = I;
    static constexpr const char* name    = T::__type_info__::method_names[I];
    using type                           = typename packtl::get<I, typename T::__type_info__::method_types>::type;
    static constexpr field_access access =
      field_access{packtl::get<I, typename T::__type_info__::method_access_specifiers>::value};
  };

  template <refl::Reflected T>
  constexpr std::size_t method_count = packtl::get_size<typename T::__type_info__::method_types>::value;

  template <Reflected R, template <Reflected, typename> typename Fun, typename... Args>
  auto for_each_field(Args&&... args) {
    static constexpr auto count = field_count<R>;

    auto impl = [&]<std::size_t... I>(std::index_sequence<I...>) {
      return ((Fun<R, field<R, I>>(args...)) && ...);
    };

    return impl(std::make_index_sequence<count>{});
  }
}