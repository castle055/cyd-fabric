// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  concepts.cppm
 *! \brief
 *!
 */

export module fabric.units:concepts;
import :preface;

namespace cyd::fabric::units {
  export template<typename U_FROM, typename U_TO, typename T>
  concept Convertible = requires { unit_conversion_t<U_FROM, U_TO, T>::factor; }
                        || requires { unit_conversion_t<U_TO, U_FROM, T>::factor; };

  // export {
    template <typename...>
    struct compare {
      template <typename...>
      struct equals: std::false_type {};
    };

    template <typename A, typename B>
    using compare_eq = typename compare<A>::template equals<B>;

    template <typename Arg1>
    struct compare<Arg1> {
      template <typename...>
      struct equals;

      template <typename Arg2>
      struct equals<Arg2> {
        constexpr static bool value = std::is_same_v<Arg1, Arg2>;
      };
    };

    template <template <typename...> typename Pack, typename Arg1>
    struct compare<Pack<Arg1>> {
      template <typename...>
      struct equals;

      template <typename Arg2>
      struct equals<Pack<Arg2>> {
        constexpr static bool value = compare<Arg1>::template equals<Arg2>::value;
      };

      template <typename Any>
      struct equals<Any>: std::false_type {};
    };

    template <template <typename...> typename Pack, typename... Args1>
    struct compare<Pack<Args1...>> {
      template <typename...>
      struct equals;

      template <typename Arg2, typename... Args2>
        requires (sizeof...(Args1) == (sizeof...(Args2) + 1) && (compare_eq<Arg2, Args1>::value || ...))
      struct equals<Pack<Arg2, Args2...>> {
      private:
        using tmp = typename ts::packs::take_one_out_w_predicate<typename compare<Arg2>::template equals<void>, ts::packs::pack<Args1...>>::type;
      public:
        constexpr static bool value = compare_eq<tmp, ts::packs::pack<Args2...>>::value;
      };

      template <typename Arg2, typename... Args2>
        requires (sizeof...(Args1) == (sizeof...(Args2) + 1) && (!compare_eq<Arg2, Args1>::value && ...))
      struct equals<Pack<Arg2, Args2...>>: std::false_type { };

      template <typename... Args2>
        requires (sizeof...(Args1) != sizeof...(Args2))
      struct equals<Pack<Args2...>>: std::false_type { };

      template <typename Any>
      struct equals<Any>: std::false_type {};
    };
  // }

  export template<typename U1, typename U2>
  concept SameScale = compare_eq<typename U1::scale, typename U2::scale>::value;

  export template<typename Q, typename S>
  concept Quantity = std::same_as<typename Q::unit::scale, S>;
}
