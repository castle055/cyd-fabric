// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "common.h"
#include <cassert>

import fabric.logging;

void setup() {
  LOG::INIT { }
    .TARGETS(
      // LOG::TARGETS::FILE::BUILDER {"stdout"}
      // .entry_format("{entry:timestamp} | {entry:path}:{entry:line} [{entry:function}]  {entry:message}")
      // LOG::TARGETS::FILE::BUILDER {"stderr"}
      // .entry_format("{entry:timestamp} | {entry:path}:{entry:line} [{entry:function}]  {entry:message}")
    )
    .FILTERS({
      {"test/.*", "stdout"}
    });
}

TEST("Nominal Start-up") {
  LOG::INIT { };
  return 0;
}

TEST("Nominal Start-up1") {
  LOG::print {INFO}("{}, {}!", "hello", "world");
  return 0;
}
