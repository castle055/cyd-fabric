// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/macros/units.h>
export module fabric.units:reduce_rules;
export import std;
export import fabric.ts.packs;
import :preface;
import :frac;
import :mul;

export namespace cyd::fabric::units {
  template<typename Numerator, typename Denominator>
  struct frac<Numerator, frac<Numerator, Denominator>> {
    using reduce = typename Denominator::reduce;
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/((" + Numerator::symbol() + ")/(" + Denominator::symbol() + "))")
  };

  template<typename Numerator1, typename Numerator2, typename Denominator>
  struct frac<Numerator1, frac<Numerator2, Denominator>> {
    using reduce = typename frac<mul<typename Numerator1::reduce, typename Denominator::reduce>, typename
                                 Numerator2::reduce>::reduce;
    UNIT_SYMBOL("(" + Numerator1::symbol() + ")/(" + Numerator2::symbol() + "*" + Denominator::symbol() + ")")
  };

  template<typename Numerator, typename Denominator1, typename Denominator2>
  struct frac<frac<Numerator, Denominator1>, Denominator2> {
    using reduce = typename frac<typename Numerator::reduce, mul<
                                   typename Denominator1::reduce, typename Denominator2::reduce>>::reduce;
    UNIT_SYMBOL("(" + Numerator::symbol() + "*" + Denominator1::symbol() + ")/(" + Denominator2::symbol() + ")")
  };

  template<typename Denominator, typename... Numerators>
    requires ts::packs::Contains<Denominator, Numerators...>
  struct frac<mul<Numerators...>, Denominator> {
    using reduce = typename ts::packs::take_one_out<Denominator, mul<Numerators...>>::type::reduce;
    UNIT_SYMBOL("(" + mul<Numerators...>::symbol() + ")/(" + Denominator::symbol() + ")")
  };

  template<typename Numerator, typename... Denominators>
    requires ts::packs::Contains<Numerator, Denominators...>
  struct frac<Numerator, mul<Denominators...>> {
    using reduce = typename frac<no_unit, typename ts::packs::take_one_out<
                                   Numerator, mul<Denominators...>>::type::reduce>::reduce;
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + mul<Denominators...>::symbol() + ")")
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
    using reduce = typename N::reduce;
    UNIT_SYMBOL("(" + N::symbol() + ")/(no unit)")
  };


  template<typename P>
  struct mul<P> {
    using reduce = typename P::reduce;
    UNIT_SYMBOL("(" + P::symbol() + ")")
  };

  template<>
  struct mul<no_unit, no_unit> {
    using reduce = no_unit;
    UNIT_SYMBOL("(no unit)*(no unit)")
  };

  template<typename P>
  struct mul<P, no_unit> {
    using reduce = typename P::reduce;
    UNIT_SYMBOL("(" + P::symbol() + ")*(no unit)")
  };

  template<typename P>
  struct mul<no_unit, P> {
    using reduce = typename P::reduce;
    UNIT_SYMBOL("(no unit)*(" + P::symbol() + ")")
  };

  template<typename P1, typename P2>
  struct mul<frac<P1, P2>, P2> {
    using reduce = typename P1::reduce;
    UNIT_SYMBOL("((" + P1::symbol() + ")/(" + P2::symbol() + "))*(" + P2::symbol() + ")")
  };

  template<typename P1, typename N, typename P2>
  struct mul<frac<P1, N>, P2> {
    using reduce = typename frac<mul<typename P2::reduce, typename P1::reduce>, typename N::reduce>::reduce;
    UNIT_SYMBOL("((" + P1::symbol() + ")/(" + N::symbol() + "))*(" + P2::symbol() + ")")
  };

  template<typename P1, typename P2>
  struct mul<P2, frac<P1, P2>> {
    using reduce = typename P1::reduce;
    UNIT_SYMBOL("(" + P2::symbol() + ")*((" + P1::symbol() + ")/(" + P2::symbol() + "))")
  };

  template<typename P1, typename N, typename P2>
  struct mul<P2, frac<P1, N>> {
    using reduce = typename frac<mul<typename P2::reduce, typename P1::reduce>, typename N::reduce>::reduce;
    UNIT_SYMBOL("(" + P2::symbol() + ")*((" + P1::symbol() + ")/(" + N::symbol() + "))")
  };

  template<typename P1, typename P2, typename D1, typename D2>
  struct mul<frac<P1, D1>, frac<P2, D2>> {
    using reduce = typename frac<mul<typename P1::reduce, typename P2::reduce>, mul<typename D1::reduce, typename D2::reduce>>::reduce;
    UNIT_SYMBOL("((" + P1::symbol() + ")/(" + D1::symbol() + "))*((" + P2::symbol() + ")/(" + D2::symbol() + "))")
  };
}
