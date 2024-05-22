// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//
module;
#include "cyd_fabric_modules/headers/macros/units.h"
export module fabric.units:preface;
export import std;
import fabric.templates.ratio;

namespace cyd::fabric::units {
  export struct no_scale {

  };
  export struct no_unit {
    using scale = no_scale;
    using reduce = no_unit;

    template<typename T>
    using factor = ratio<T, 1, 1>;

    UNIT_SYMBOL("(no unit)")
  };

  export template<typename U_FROM, typename U_TO, typename T>
  struct unit_conversion_t {
  };
}
