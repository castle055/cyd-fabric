// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:dose_equivalent;
export import fabric.units;

import :energy;
import :mass;

export namespace cyd::fabric::units {
  DERIVED_SCALE(dose_equivalent, mul<energy::scale, mass::scale>) {
    METRIC_SCALE(sieverts, "Sv", 1, 1)
  }
}
