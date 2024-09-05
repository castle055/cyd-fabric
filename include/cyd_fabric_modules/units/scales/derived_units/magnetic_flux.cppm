// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"

export module fabric.units.scales:magnetic_flux;
export import fabric.units.core;

import :emf;
import :time;

export namespace fabric::units {
  DERIVED_SCALE(magnetic_flux, mul<emf::scale, time::scale>) {
    METRIC_SCALE(webers, "Wb", 1, 1)
  }
}
