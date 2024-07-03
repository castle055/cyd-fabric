// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/macros/units.h>
export module fabric.units:frac;
import std;
import fabric.ts.packs;
export import fabric.templates.ratio;

namespace cyd::fabric::units {
  export template<typename Numerator, typename Denominator>
  struct frac {
    using scale  = frac<typename Numerator::scale, typename Denominator::scale>;
    // using reduce = frac<typename Numerator::reduce, typename Denominator::reduce>;

    template<typename T>
    using factor = ratio<
      T,
      (long)(Numerator::template factor<T>::numerator * Denominator::template factor<T>::denominator),
      (long)(Numerator::template factor<T>::denominator * Denominator::template factor<T>::numerator)
    >;

    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator::symbol() + ")")
  };
}
