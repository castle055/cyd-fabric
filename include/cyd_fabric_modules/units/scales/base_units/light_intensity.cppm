// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:light_intensity;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(light_intensity) {
    METRIC_SCALE(candela, "cd", 1, 1)
  }
}
