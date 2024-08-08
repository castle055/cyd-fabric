// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:electric_charge;
export import fabric.units;

import :electric_current;
import :time;

export namespace cyd::fabric::units {
  DERIVED_SCALE(electric_charge, mul<electric_current::scale, time::scale>) {
    METRIC_SCALE(coulombs, "C", 1, 1)
  }
}
