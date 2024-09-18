// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  apply.cppm
 *! \brief 
 *!
 */

export module fabric.ts.apply;
export import fabric.ts.packs;

// import fabric.refl;

export namespace fabric::ts {
  template<typename...>
  struct with_type;
}

namespace fabric::ts::impl {
  template<typename...>
  struct with_type;
}

namespace fabric::ts {
  template<typename... Args>
  struct with_type: impl::with_type<packs::pack<Args...>> {
  };

  template<template <typename...> typename Pack, typename... Args>
  struct with_type<Pack<Args...>>: impl::with_type<Pack<Args...>> {
  };

  template<template <std::size_t...> typename Pack, std::size_t... Args>
  struct with_type<Pack<Args...>>: impl::with_type<Pack<Args...>> {
  };
}

namespace fabric::ts::impl {
  template<typename... Args>
  struct with_type<packs::pack<Args...>> {
    template<template <typename...> typename Transform>
    using apply = with_type<packs::pack<typename Transform<Args...>::type>>;

    template<template <typename...> typename Transform>
    using apply_as_pack = with_type<typename Transform<packs::pack<Args...>>::type>;

    using done = std::conditional_t<sizeof...(Args) == 1, typename packs::get_first<Args...>::type, packs::pack<Args...>>;
  };

  template<template <typename...> typename Pack, typename... Args>
  struct with_type<Pack<Args...>> {
    template<template <typename...> typename Transform>
    using apply = with_type<Pack<typename Transform<Args...>::type>>;

    template<template <typename...> typename Transform>
    using apply_as_pack = with_type<typename Transform<Pack<Args...>>::type>;

    using done = Pack<Args...>;
  };

  template<template <std::size_t...> typename Pack, std::size_t... Args>
  struct with_type<Pack<Args...>> {
    template<template <std::size_t...> typename Transform>
    using apply = with_type<Pack<Transform<Args...>::value>>;

    template<template <typename...> typename Transform>
    using apply_as_pack = with_type<typename Transform<Pack<Args...>>::type>;

    using done = Pack<Args...>;
  };
}
