// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:radionuclide_activity;
export import fabric.units.core;

import :time;

export namespace fabric::units {
  DERIVED_SCALE(radionuclide_activity, frac<no_scale, time::scale>) {
    METRIC_SCALE(becquerels, "Bq", 1, 1)
  }
}
