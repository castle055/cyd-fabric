// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:volume;
export import fabric.units.core;

import :distance;

export namespace cyd::fabric::units {
  DERIVED_SCALE(volume, mul<distance::scale, distance::scale, distance::scale>) {
    METRIC_SCALE(liters, "L", 1, 1)
  }
}
