// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:pressure;
export import fabric.units;

import :mass;
import :distance;
import :time;
import :force;

export namespace cyd::fabric::units {
  DERIVED_SCALE(pressure, frac<force::scale, mul<distance::scale, distance::scale>>) {
    METRIC_SCALE(pascals, "Pa", 1, 1);
  }
}
