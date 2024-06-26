// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:angle;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(angle) {
    UNIT(degrees, "deg", 1,1)
    UNIT(radians, "rad", 180*10000000000000000,31415926535893238)
  }
}
