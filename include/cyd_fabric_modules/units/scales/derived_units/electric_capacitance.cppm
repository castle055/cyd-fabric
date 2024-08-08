// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:electric_capacitance;
export import fabric.units;

import :emf;
import :electric_charge;

export namespace cyd::fabric::units {
  DERIVED_SCALE(electric_capacitance, frac<electric_charge::scale, emf::scale>) {
    METRIC_SCALE(farads, "F", 1, 1)
  }
}
