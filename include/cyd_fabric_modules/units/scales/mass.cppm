//
// Created by castle on 5/11/24.
//
module;
#include "cyd_fabric_modules/headers/units_includes.h"
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
