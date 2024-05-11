//
// Created by castle on 5/11/24.
//
module;
#include "cyd_fabric_modules/headers/units_includes.h"
export module fabric.units.scales:speed;
export import fabric.units;

import :distance;
import :time;

export namespace cyd::fabric::units {
  namespace speed {
    using scale = frac<distance::scale, time::scale>;
    template<typename Q> concept quantity = Quantity<Q, scale>;

    using m_s = frac<distance::meters, time::seconds>;
  }
}
