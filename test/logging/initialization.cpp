// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "common.h"
#include <cassert>

import fabric.logging;

void setup() {
}

TEST("Default Initialization") {
  LOG::INIT { };
  return 0;
}

TEST("Filter") {
  LOG::INIT { }
    .FILTERS({{"", "stdout"}});
  return 0;
}

TEST("Filter - Unknown Target") {
  try {
    LOG::INIT { }
      .FILTERS({{"", "this target doesn't exit"}});
  } catch (std::runtime_error &err) {
    assert(std::string{err.what()} == "Target ID \"this target doesn't exit\" does not exists");
    return 0;
  }
  std::cerr
    << "[ERROR] "
    << "The target use in the filter should not exist. It should have thrown a `runtime_error`."
    << std::endl;
  return 1;
}

TEST("Target Configuration") {
  LOG::INIT { }
    .TARGETS(
      // LOG::TARGETS::FILE::BUILDER {"stdout"}
      // .entry_format("{entry:timestamp} | {entry:path}:{entry:line} [{entry:function}]  {entry:message}")
      // LOG::TARGETS::FILE::BUILDER {"stderr"}
      // .entry_format("{entry:timestamp} | {entry:path}:{entry:line} [{entry:function}]  {entry:message}")
    );
  return 0;
}

