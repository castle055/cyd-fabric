// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/units_preface.h>
export module fabric.units:reduce_rules;
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
}