// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:electric_conductance;
export import fabric.units;

import :electric_resistance;

export namespace cyd::fabric::units {
  DERIVED_SCALE(electric_conductance, frac<no_scale, electric_resistance::scale>) {
    METRIC_SCALE(siemens, "S", 1, 1)
  }
}
