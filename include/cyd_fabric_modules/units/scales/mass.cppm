// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:mass;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(mass, {
    UNIT(grams, "g", 1,1)

    //! Metric
    UNIT(decigrams, "dg", 1,10)
    UNIT(centigrams, "cg", 1,100)
    UNIT(milligrams, "mg", 1,1000)
    UNIT(micrograms, "ug", 1,1000000)
    UNIT(nanograms, "ng", 1,1000000000)

    UNIT(decagrams, "Dg", 10,1)
    UNIT(hectograms, "hg", 100,1)
    UNIT(kilograms, "kg", 1000, 1)
    UNIT(ton, "T", 1000000, 1)

  })
}
