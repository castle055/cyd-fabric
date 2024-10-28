// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:luminous_flux;
export import fabric.units.core;

import :light_intensity;
import :solid_angle;

export namespace fabric::units {
  DERIVED_SCALE(luminous_flux, mul<light_intensity::scale, solid_angle::scale>) {
    METRIC_SCALE(lumen, "lm", 1, 1)
  }
}
