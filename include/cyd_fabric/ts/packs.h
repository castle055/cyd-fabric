// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PACKS_H
#define PACKS_H

#include <type_traits>
#include <concepts>


//! @brief Concepts and Operations related to template parameter packs.
namespace cyd::fabric::ts::packs {
  template<typename...>
  struct pack;
}


//! <h2> CONCEPTS </h2>
namespace cyd::fabric::ts::packs {
  //! Contains
  template<typename What, typename... Args>
  concept Contains = (std::is_same_v<What, Args> || ...);
}

//! <h2> OPERATORS </h2>
namespace cyd::fabric::ts::packs {
  template<typename...>
  struct get_first;
  template<typename...>
  struct append;
  template<typename...>
  struct prepend;
  template<typename...>
  struct take_one_out;
  template<typename...>
  struct substitute;

  namespace impl {
    template<typename... Args>
    struct take_one_out;

    template<typename... Args>
    struct substitute;
  } //
}

namespace cyd::fabric::ts::packs {
  //! get_first
  template<typename First, typename... Rest>
  struct get_first<First, Rest...> {
    using type = First;
  };

  template<template <typename...> typename Pack, typename First, typename... Rest>
  struct get_first<Pack<First, Rest...>> {
    using type = First;
  };


  //! append
  template<typename... What>
  struct append {
    template<typename... T>
    struct to;

    template<template <typename...> typename Pack, typename... Args>
    struct to<Pack<Args...>> {
      using type = Pack<Args..., What...>;
    };
  };

  template<typename What, template <typename...> typename From, typename... Args>
  struct append<What, From<Args...>> {
    using type = From<Args..., What>;
  };


  //! prepend
  template<typename... What>
  struct prepend {
    template<typename... T>
    struct to;

    template<template <typename...> typename Pack, typename... Args>
    struct to<Pack<Args...>> {
      using type = Pack<What..., Args...>;
    };
  };

  template<typename What, template <typename...> typename From, typename... Args>
  struct prepend<What, From<Args...>> {
    using type = From<What, Args...>;
  };


  //! take_one_out
  template<typename What, template <typename...> typename From, typename... Args>
    requires Contains<What, Args...>
  struct take_one_out<What, From<Args...>> {
    using type = typename impl::take_one_out<What, From<Args...>>::type;
  };


  //! substitute
  template<typename What, typename As, template <typename...> typename From, typename... Args>
    requires Contains<What, Args...>
  struct substitute<What, As, From<Args...>> {
    using type = typename impl::substitute<What, As, From<Args...>>::type;
  };


  template<typename What, typename As, typename From>
    requires std::same_as<What, From>
  struct substitute<What, As, From> {
    using type = As;
  };

  template<typename What, typename As, typename From>
    requires (!std::same_as<What, From>)
  struct substitute<What, As, From> {
    using type = From;
  };
}

namespace cyd::fabric::ts::packs::impl {
  //! take_one_out
  template<typename What, template <typename, typename...> typename From, typename A1, typename... Args>
    requires std::same_as<What, A1>
  struct take_one_out<What, From<A1, Args...>> {
    using type = From<Args...>;
  };

  template<
    typename What,
    template <typename, typename...> typename From,
    typename A1,
    typename... Args
  > requires (!std::same_as<What, A1>)
  struct take_one_out<What, From<A1, Args...>> {
    using type = typename prepend<A1, typename take_one_out<What, From<Args...>>::type>::type;
  };


  //! substitute
  template<
    typename What,
    typename As,
    template <typename> typename From,
    typename A1
  > requires std::same_as<What, A1>
  struct substitute<What, As, From<A1>> {
    using type = As;
  };

  template<
    typename What,
    typename As,
    template <typename> typename From,
    typename A1
  > requires (!std::same_as<What, A1>)
  struct substitute<What, As, From<A1>> {
    using type = A1;
  };

  template<
    typename What,
    typename As,
    template <typename, typename...> typename From,
    typename A1,
    typename... Args
  > requires std::same_as<What, A1>
  struct substitute<What, As, From<A1, Args...>> {
    using type = typename prepend<As, From<typename substitute<What, As, From<Args...>>::type>>::type;
  };

  template<
    typename What,
    typename As,
    template <typename, typename...> typename From,
    typename A1,
    typename... Args
  > requires (!std::same_as<What, A1>)
  struct substitute<What, As, From<A1, Args...>> {
    using type = typename prepend<A1, From<typename substitute<What, As, From<Args...>>::type>>::type;
  };
}

#endif //PACKS_H
