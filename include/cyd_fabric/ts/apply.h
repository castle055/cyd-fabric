// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPLY_H
#define APPLY_H

#include "packs.h"

namespace cyd::fabric::ts {
  template<typename>
  struct with_type;

  namespace impl {
    template<typename>
    struct with_type;
  } //
}

namespace cyd::fabric::ts {
  template<typename... Args>
  struct with_type<Args...>: impl::with_type<packs::pack<Args...>> {
  };

  template<template <typename...> typename Pack, typename... Args>
  struct with_type<Pack<Args...>>: impl::with_type<Pack<Args...>> {
  };
}

namespace cyd::fabric::ts::impl {
  template<template <typename...> typename Pack, typename... Args>
  struct with_type<Pack<Args...>> {
    template<template <typename...> typename Transform>
    using apply = with_type<Pack<typename Transform<Args...>::type>>;

    template<template <typename...> typename Transform>
    using apply_as_pack = with_type<typename Transform<Pack<Args...>>::type>;

    using done = std::conditional_t<sizeof...(Args) == 1, typename packs::get_first<Args...>::type, Pack<Args...>>;
  };
}


#endif //APPLY_H
