// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:emf;
export import fabric.units.core;

import :power;
import :electric_current;

export namespace fabric::units {
  DERIVED_SCALE(emf, frac<power::scale, electric_current::scale>) {
    METRIC_SCALE(volts, "V", 1, 1)
  }
}
