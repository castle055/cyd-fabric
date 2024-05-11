//
// Created by castle on 5/11/24.
//
module;
#include "cyd_fabric_modules/headers/units_includes.h"
export module fabric.units.scales:angle;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(angle, {
    UNIT(degrees, "deg", 1,1)

    //! Metric
    UNIT(radians, "rad", 180*10000000000000000,31415926535893238)
  })
}
