// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  concepts.cppm
 *! \brief
 *!
 */

export module fabric.units.core:concepts;
import :reduce_rules;
import :preface;
export import fabric.ts.packs;

namespace packs = fabric::ts::packs;

namespace fabric::units {
  export template<typename U_FROM, typename U_TO, typename T>
  concept Convertible = requires { unit_conversion_t<U_FROM, U_TO, T>::factor; }
                        || requires { unit_conversion_t<U_TO, U_FROM, T>::factor; };

  // export template<typename S_FROM, typename S_TO, typename U_FROM, typename U_TO, typename T>
  // concept ConvertibleScales = requires { scale_conversion_t<S_FROM, S_TO>::template forward<U_FROM, U_TO, T>; }
                              // || requires { scale_conversion_t<S_TO, S_FROM>::template backward<U_FROM, U_TO, T>; };
  export template<typename S_FROM, typename S_TO, typename U_FROM, typename U_TO, typename T>
  concept ConvertibleScales = requires { scale_conversion_t<S_FROM, S_TO>{}; }
                              || requires { scale_conversion_t<S_TO, S_FROM>{}; };

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

  export template <typename Scale>
  struct reduce_scale {
    using type = Scale;
  };

  export template <typename Scale>
  requires requires {
    typename reduce_impl<Scale>::type;
  }
  struct reduce_scale<Scale> {
    using type = reduce<Scale>;
  };

  export template<typename U1, typename U2>
  concept CompareScales = compare_eq<typename reduce_scale<U1>::type, typename reduce_scale<U2>::type>::value;

  export template<typename U1, typename U2>
  concept SameScale = CompareScales<typename U1::scale, typename U2::scale>;

  export template<typename Q, typename S>
  concept Quantity = std::same_as<typename Q::unit::scale, S>;

  export template<typename U, typename T>
  struct quantity_t;

  export template <typename T>
  using is_quantity = packs::is_type<quantity_t, T>;

  export template <typename T>
  constexpr bool is_quantity_v = is_quantity<T>::value;

  export template <typename T>
  concept QuantityConcept = is_quantity_v<T>;
}
