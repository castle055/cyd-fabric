// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:electric_current;
export import fabric.units.core;

export namespace cyd::fabric::units {
  SCALE(electric_current) {
    METRIC_SCALE(ampere, "A", 1, 1)
  }
}
