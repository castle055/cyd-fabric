// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:illuminance;
export import fabric.units;

import :luminous_flux;
import :distance;

export namespace cyd::fabric::units {
  DERIVED_SCALE(illuminance, frac<luminous_flux::scale, mul<distance::scale, distance::scale>>) {
    METRIC_SCALE(lux, "lx", 1, 1)
  }
}
