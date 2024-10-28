// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:catalytic_activity;
export import fabric.units.core;

import :substance;
import :time;

export namespace fabric::units {
  DERIVED_SCALE(catalytic_activity, frac<substance::scale, time::scale>) {
    METRIC_SCALE(katals, "kat", 1, 1)
  }
}
