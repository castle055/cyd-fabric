// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:power;
export import fabric.units.core;

import :energy;
import :time;

export namespace cyd::fabric::units {
  DERIVED_SCALE(power, frac<energy::scale, time::scale>) {
    METRIC_SCALE(watts, "W", 1, 1)
  }
}
