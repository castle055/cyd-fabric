// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:solid_angle;
export import fabric.units;

import :angle;

export namespace cyd::fabric::units {
  DERIVED_SCALE(solid_angle, mul<angle::scale, angle::scale>) {
    METRIC_SCALE(steradian, "sr", 1, 1)
  }
}
