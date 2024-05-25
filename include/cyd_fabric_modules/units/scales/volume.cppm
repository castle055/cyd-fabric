// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:volume;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(volume, {
    UNIT(liters, "L", 1,1)

    //! Metric
    UNIT(deciliters, "dL", 1,10)
    UNIT(centiliters, "cL", 1,100)
    UNIT(milliliters, "mL", 1,1000)
    UNIT(microliters, "uL", 1,1000000)
    UNIT(nanoliters, "nL", 1,1000000000)

    UNIT(decaliters, "DL", 10,1)
    UNIT(hectoliters, "hL", 100,1)
    UNIT(kiloliters, "kL", 1000, 1)
    UNIT(megaliters, "ML", 1000000, 1)

  })
}
