// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:absorbed_dose;
export import fabric.units.core;

import :energy;
import :mass;

export namespace fabric::units {
  DERIVED_SCALE(absorbed_dose, mul<energy::scale, mass::scale>) {
    METRIC_SCALE(grays, "Gy", 1, 1)
  }
}
