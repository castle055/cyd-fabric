// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

//! Syntax sugar - Apply reduce
#define R(...) typename __VA_ARGS__::reduce

#include <cyd_fabric_modules/headers/macros/units.h>
export module fabric.units:reduce_rules;
export import std;
export import fabric.ts.packs;
import :preface;
import :frac;
import :mul;

export namespace cyd::fabric::units {
  //* frac
  template<typename Numerator, typename Denominator>
  requires (!is_frac<Numerator>::value)
  struct frac<Numerator, frac<Numerator, Denominator>> {
    using reduce = R(Denominator);
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/((" + Numerator::symbol() + ")/(" + Denominator::symbol() + "))")
  };

  template<typename Numerator1, typename Numerator2, typename Denominator>
  requires (!is_frac<Numerator1>::value)
  struct frac<Numerator1, frac<Numerator2, Denominator>> {
    using reduce = R(frac<R(mul<R(Numerator1), R(Denominator)>), R(Numerator2)>);
    UNIT_SYMBOL("(" + Numerator1::symbol() + "*" + Numerator2::symbol() + ")/(" + Denominator::symbol() + ")")
  };

  template<typename Numerator, typename Denominator1, typename Denominator2>
  requires (!is_frac<Denominator2>::value)
  struct frac<frac<Numerator, Denominator1>, Denominator2> {
    using reduce = R(frac<R(Numerator), R(mul<R(Denominator1), R(Denominator2)>)>);
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator1::symbol() + "*" + Denominator2::symbol() + ")")
  };

  template<typename N1, typename N2, typename D1, typename D2>
  requires (!std::is_same_v<N1, N2> && !std::is_same_v<D1, D2>)
  struct frac<frac<N1, D1>, frac<N2, D2>> {
    using reduce = R(frac<R(mul<R(N1), R(D2)>), R(mul<R(D1), R(N2)>)>);
    UNIT_SYMBOL("(" + N1::symbol() + "*" + D2::symbol() + ")/(" + D1::symbol() + "*" + N2::symbol() + ")")
  };

  template<typename Denominator, typename... Numerators>
    requires ts::packs::Contains<Denominator, Numerators...>
  struct frac<mul<Numerators...>, Denominator> {
    using reduce = R(ts::packs::take_one_out<Denominator, mul<Numerators...>>::type);
    UNIT_SYMBOL("(" + mul<Numerators...>::symbol() + ")/(" + Denominator::symbol() + ")")
  };

  template<typename Numerator, typename... Denominators>
    requires ts::packs::Contains<Numerator, Denominators...>
  struct frac<Numerator, mul<Denominators...>> {
    using reduce = R(frac<no_unit, R(ts::packs::take_one_out<Numerator, mul<Denominators...>>::type)>);
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + mul<Denominators...>::symbol() + ")")
  };

  template<typename Numerator, typename Denominator>
    requires (is_mul<Numerator>::value && is_mul<Denominator>::value && ts::packs::share_items<Numerator, Denominator>::value)
  struct frac<Numerator, Denominator> {
  private:
    using cancelled_numerator   = typename cancel_out_many<R(Numerator), R(Denominator)>::type;
    using cancelled_denominator = typename cancel_out_many<R(Denominator), R(Numerator)>::type;
  public:
    using reduce = frac<R(cancelled_numerator), R(cancelled_denominator)>;
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator::symbol() + ")")
  };

  template<typename Numerator>
  struct frac<Numerator, Numerator> {
    using reduce = no_unit;
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Numerator::symbol() + ")")
  };

  template<>
  struct frac<no_unit, no_unit> {
    using reduce = no_unit;
    UNIT_SYMBOL("(no unit)/(no unit)")
  };

  template<typename N>
  struct frac<N, no_unit> {
    using reduce = R(N);
    UNIT_SYMBOL("(" + N::symbol() + ")/(no unit)")
  };

  //* mul
  template<typename P>
  struct mul<P> {
  using reduce = R(P);
  UNIT_SYMBOL(P::symbol())
  };

  template<>
  struct mul<no_unit, no_unit> {
    using reduce = no_unit;
    UNIT_SYMBOL("(no unit)*(no unit)")
  };

  template<typename P>
  struct mul<P, no_unit> {
    using reduce = R(P);
    UNIT_SYMBOL("(" + P::symbol() + ")*(no unit)")
  };

  template<typename P>
  struct mul<no_unit, P> {
    using reduce = R(P);
    UNIT_SYMBOL("(no unit)*(" + P::symbol() + ")")
  };

  template<typename P1, typename P2>
  struct mul<frac<P1, P2>, P2> {
    using reduce = R(P1);
    UNIT_SYMBOL("((" + P1::symbol() + ")/(" + P2::symbol() + "))*(" + P2::symbol() + ")")
  };

  template<typename P1, typename N, typename P2>
  struct mul<frac<P1, N>, P2> {
    using reduce = R(frac<R(mul<R(P2), R(P1)>), R(N)>);
    UNIT_SYMBOL("((" + P1::symbol() + ")/(" + N::symbol() + "))*(" + P2::symbol() + ")")
  };

  template<typename P1, typename P2>
  struct mul<P2, frac<P1, P2>> {
    using reduce = R(P1);
    UNIT_SYMBOL("(" + P2::symbol() + ")*((" + P1::symbol() + ")/(" + P2::symbol() + "))")
  };

  template<typename P1, typename N, typename P2>
  struct mul<P2, frac<P1, N>> {
    using reduce = R(frac<R(mul<R(P2), R(P1)>), R(N)>);
    UNIT_SYMBOL("(" + P2::symbol() + ")*((" + P1::symbol() + ")/(" + N::symbol() + "))")
  };

  template<typename P1, typename P2, typename D1, typename D2>
  struct mul<frac<P1, D1>, frac<P2, D2>> {
    using reduce = R(frac<R(mul<R(P1), R(P2)>), R(mul<R(D1), R(D2)>)>);
    UNIT_SYMBOL("((" + P1::symbol() + ")/(" + D1::symbol() + "))*((" + P2::symbol() + ")/(" + D2::symbol() + "))")
  };
}
