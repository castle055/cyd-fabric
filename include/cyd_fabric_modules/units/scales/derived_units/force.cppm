// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:force;
export import fabric.units.core;

import :mass;
import :distance;
import :time;

export namespace cyd::fabric::units {
  DERIVED_SCALE(force, frac<mul<mass::scale, distance::scale>, mul<time::scale, time::scale>>) {
    UNIT(newtons, "N", 1, 1);
  }
}
