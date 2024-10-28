// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/macros/units.h>
export module fabric.units.core:frac;
import std;
import fabric.ts.packs;
export import fabric.templates.ratio;

export namespace fabric::units {
  template<typename Numerator, typename Denominator>
  struct frac {
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator::symbol() + ")")
  };

  template<typename Numerator, typename Denominator>
  requires requires {
    typename Numerator::scale;
    typename Denominator::scale;
  }
  struct frac<Numerator, Denominator> {
    using scale  = frac<typename Numerator::scale, typename Denominator::scale>;

    template<typename T>
    using factor = ratio<
      T,
      (long)(Numerator::template factor<T>::numerator * Denominator::template factor<T>::denominator),
      (long)(Numerator::template factor<T>::denominator * Denominator::template factor<T>::numerator)
    >;

    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator::symbol() + ")")
  };
}
