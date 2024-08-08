// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:energy;
export import fabric.units;

import :force;
import :distance;

export namespace cyd::fabric::units {
  DERIVED_SCALE(energy, mul<force::scale, distance::scale>) {
    METRIC_SCALE(joule, "J", 1, 1)
  }
}
