// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:electric_resistance;
export import fabric.units.core;

import :emf;
import :electric_current;

export namespace cyd::fabric::units {
  DERIVED_SCALE(electric_resistance, frac<emf::scale, electric_current::scale>) {
    METRIC_SCALE(ohms, "ohm", 1, 1)
  }
}
