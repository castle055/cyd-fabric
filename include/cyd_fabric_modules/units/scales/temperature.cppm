// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:temperature;
export import fabric.units;

export namespace cyd::fabric::units {
  namespace temperature {
    namespace scales {
      SCALE(kelvin) {}
      SCALE(celsius) {}
      SCALE(fahrenheit) {}
    }

    UNIT_IN_SCALE(scales::kelvin::scale, kelvin, "K", 1, 1)

    UNIT_IN_SCALE(scales::celsius::scale, celsius, "C", 1, 1)

    UNIT_IN_SCALE(scales::fahrenheit::scale, fahrenheit, "F", 1, 1)
  }
}
