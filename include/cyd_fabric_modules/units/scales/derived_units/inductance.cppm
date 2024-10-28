// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:inductance;
export import fabric.units.core;

import :electric_current;
import :magnetic_flux;

export namespace fabric::units {
  DERIVED_SCALE(inductance, frac<magnetic_flux::scale, electric_current::scale>) {
    METRIC_SCALE(henries, "H", 1, 1)
  }
}
