// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:substance;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(substance) {
    METRIC_SCALE(mole, "mol", 1, 1)
  }
}
