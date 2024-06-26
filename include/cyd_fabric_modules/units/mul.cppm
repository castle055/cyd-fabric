// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/macros/units.h>

export module fabric.units:mul;
import std;
import fabric.ts.packs;
import :preface;
export import fabric.templates.ratio;

export namespace cyd::fabric::units {
  template <typename...>
  struct mul;

  template <typename M>
  using is_mul = ts::packs::is_type<mul, M>;

  template <typename... Products>
  struct mul {
    using scale  = mul<typename Products::scale...>;
    using reduce = typename ts::packs::flatten<mul<typename Products::reduce...>>::type;

    template <typename T>
    using factor = ratio<
      T,
      (long)(1 * ... * Products::template factor<T>::numerator),
      (long)(1 * ... * Products::template factor<T>::denominator)
    >;

    using first_product = typename ts::packs::get_first<Products...>::type;
    using tail_products = typename ts::packs::take_one_out<first_product, mul>::type;

    UNIT_SYMBOL(first_product::symbol() + "*" + tail_products::symbol())
  };

  template <typename Mul, typename CancelProduct>
  using cancel_out = typename ts::packs::take_one_out<CancelProduct, Mul>::type;

  template <typename...>
  struct cancel_out_many {};

  template <typename Mul, typename CP>
  struct cancel_out_many<Mul, CP> {
    using type = cancel_out<Mul, CP>;
  };

  template <typename Mul, typename CP1, typename... CancelProducts>
  struct cancel_out_many<Mul, CP1, CancelProducts...> {
  private:
    using cancelled      = typename ts::packs::take_one_out<CP1, Mul>::type;
    using cancelled_many = std::conditional_t<
      (sizeof...(CancelProducts) > 1),
      cancel_out_many<cancelled, CancelProducts...>,
      ts::packs::take_one_out<typename ts::packs::get_first<CancelProducts...>::type, cancelled>>;

    using result = std::conditional_t<
      sizeof...(CancelProducts) == 0,
      cancelled,
      typename cancelled_many::type>;

  public:
    using type = result;
  };

  template <typename Mul, template <typename...> typename Pack, typename... CancelProducts>
  struct cancel_out_many<Mul, Pack<CancelProducts...>> {
    using type = typename cancel_out_many<Mul, CancelProducts...>::type;
  };

}
