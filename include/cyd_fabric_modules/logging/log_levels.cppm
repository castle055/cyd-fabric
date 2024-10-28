// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  log_levels.cppm
 *! \brief 
 *!
 */
export module fabric.logging.levels;
import std;

export namespace LOG {
  struct LEVEL {
    const char* name;
    const int priority;
  };
}

export constexpr LOG::LEVEL DEBUG {"DEBUG", 0};
export constexpr LOG::LEVEL INFO  {"INFO ", 1};
export constexpr LOG::LEVEL WARN  {"WARN ", 2};
export constexpr LOG::LEVEL ERROR {"ERROR", 3};
export constexpr LOG::LEVEL FATAL {"FATAL", 4};
