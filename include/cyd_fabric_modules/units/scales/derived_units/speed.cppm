// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units.scales:speed;
export import fabric.units;

import :distance;
import :time;

export namespace cyd::fabric::units {
  namespace speed {
    using scale = frac<distance::scale, time::scale>;
    template<typename Q> concept quantity = Quantity<Q, scale>;

    struct m_s: frac<distance::meters, time::seconds> {};
  }
}
