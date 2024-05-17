// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  log_levels.cppm
 *! \brief 
 *!
 */

export module fabric.logging:log_levels;

export namespace LOG {
  struct LEVEL {
    const int priority;
  };
}

export constexpr LOG::LEVEL DEBUG {0};
export constexpr LOG::LEVEL INFO {1};
export constexpr LOG::LEVEL WARN {2};
export constexpr LOG::LEVEL ERROR {3};
