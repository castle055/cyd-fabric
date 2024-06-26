// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  packs.cppm
 *! \brief 
 *!
 */

export module fabric.ts.packs;
export import std;

//! @brief Concepts and Operations related to template parameter packs.
export namespace cyd::fabric::ts::packs {
  template<typename...>
  struct pack;
}


//! <h2> CONCEPTS </h2>
export namespace cyd::fabric::ts::packs {
  //! Contains
  template<typename What, typename... Args>
  concept Contains = (std::is_same_v<What, Args> || ...);
}

//! <h2> CONDITIONS </h2>
export namespace cyd::fabric::ts::packs {
  template <template <typename...> typename, typename...>
  struct is_type;
  template <typename...>
  struct has_item;
  template <typename...>
  struct share_items;
}

//! <h2> OPERATORS </h2>
export namespace cyd::fabric::ts::packs {
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
  template<typename...>
  struct flatten;
}

namespace cyd::fabric::ts::packs::impl {
  template<typename...>
  struct take_one_out;
  template<typename...>
  struct substitute;
  template<typename, std::size_t>
  struct flatten;
}

namespace cyd::fabric::ts::packs {
  //! is_type
  template <template <typename...> typename, typename...>
  struct is_type: std::false_type { };

  template <template <typename...> typename Pack, class... Args>
  struct is_type<Pack, Pack<Args...>>: std::true_type { };

  //! has_item
  template <typename ...>
  struct has_item: std::false_type {};

  template <typename Item, template <typename...> typename Pack, typename ...Args>
  requires ts::packs::Contains<Item, Args...>
  struct has_item<Item, Pack<Args...>>: std::true_type {};

  //! share_items
  template <typename ...>
  struct share_items: std::false_type { };

  template <typename Pack1, template <typename...> typename Pack2, typename... Pack2Args>
  requires (has_item<Pack2Args, Pack1>::value || ...)
  struct share_items<Pack1, Pack2<Pack2Args...>>: std::true_type { };
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
    requires (!Contains<What, Args...>)
  struct take_one_out<What, From<Args...>> {
    using type = From<Args...>;
  };
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

  //! flatten
  template <template <typename...> typename Pack, typename... Args>
  struct flatten<Pack<Args...>> {
    using type = typename impl::flatten<Pack<Args...>, sizeof...(Args)>::type;
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

  //! flatten
  template <template <typename...> typename Pack, typename... Args>
  struct flatten<Pack<Args...>, 0> {
    using type = Pack<Args...>;
  };

  template <template <typename...> typename Pack, typename P1, typename... PRest, std::size_t N>
    requires (is_type<Pack, P1>::value && N > 0)
  struct flatten<Pack<P1, PRest...>, N> {
  private:
    using appended = typename append<PRest...>::template to<P1>;
  public:
    using type = typename packs::flatten<typename appended::type>::type;
  };

  template <template <typename...> typename Pack, typename P1, typename... PRest, std::size_t N>
    requires (!is_type<Pack, P1>::value && N > 0)
  struct flatten<Pack<P1, PRest...>, N> {
    using type = typename flatten<Pack<PRest..., P1>, N - 1>::type;
  };
}
