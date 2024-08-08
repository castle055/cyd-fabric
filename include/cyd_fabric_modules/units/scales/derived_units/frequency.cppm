// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:frequency;
export import fabric.units.core;

import :time;

export namespace cyd::fabric::units {
  DERIVED_SCALE(frequency, frac<no_scale, time::scale>) {
    METRIC_SCALE(hertz, "Hz", 1, 1)
  }
}
