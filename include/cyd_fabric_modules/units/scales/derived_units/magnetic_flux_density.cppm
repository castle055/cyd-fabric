// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:magnetic_flux_density;
export import fabric.units.core;

import :magnetic_flux;
import :distance;

export namespace cyd::fabric::units {
  DERIVED_SCALE(magnetic_flux_density, frac<magnetic_flux::scale, mul<distance::scale, distance::scale>>) {
    METRIC_SCALE(teslas, "T", 1, 1)
  }
}
