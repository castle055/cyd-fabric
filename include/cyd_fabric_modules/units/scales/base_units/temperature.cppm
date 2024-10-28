// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:temperature;
export import fabric.units.core;

export namespace fabric::units {
  namespace temperature {
    namespace scales {
      SCALE(kelvin) {}
      SCALE(celsius) {}
      SCALE(fahrenheit) {}
    }

    UNIT_IN_SCALE(scales::kelvin::scale, kelvin, "K", 1, 1)

    UNIT_IN_SCALE(scales::celsius::scale, celsius, "C", 1, 1)
    UNIT_IN_SCALE(scales::celsius::scale, millicelsius, "mC", 1, 1000)

    UNIT_IN_SCALE(scales::fahrenheit::scale, fahrenheit, "F", 1, 1)
  }
}

using namespace fabric::units::temperature;
export
{
//@formatter:off
  SCALE_CONVERSION(scales::kelvin::scale, scales::celsius::scale)
    SCALE_FORWARD_CONVERSION(it - 273.15) SCALE_BACKWARD_CONVERSION(it + 273.15)

  SCALE_CONVERSION(scales::celsius::scale, scales::fahrenheit::scale)
    SCALE_FORWARD_CONVERSION((it * 9 / 5) + 32) SCALE_BACKWARD_CONVERSION((it - 32) * 5 / 9)

  SCALE_CONVERSION(scales::kelvin::scale, scales::fahrenheit::scale)
    SCALE_FORWARD_CONVERSION(((it - 273.15) * 9 / 5) + 32) SCALE_BACKWARD_CONVERSION(((it - 32) * 5 / 9) + 273.15)
//@formatter:on
}
