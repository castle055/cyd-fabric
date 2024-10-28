// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:mass;
export import fabric.units.core;

export namespace fabric::units {
  SCALE(mass) {
    METRIC_SCALE(grams, "g", 1, 1000)
    UNIT(ton, "T", 1000, 1)
  }
}
