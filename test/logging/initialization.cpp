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
    .filters({{"", "stdout"}});
  return 0;
}

TEST("Filter - Unknown Target") {
  try {
    LOG::INIT { }
      .filter()
      .path("")
      .levels({ERROR, {DEBUG, WARN}})["this target doesn't exist"]
      .filter({"", "this target doesn't exist"});
  } catch (std::runtime_error &err) {
    assert(std::string{err.what()} == "Target ID \"this target doesn't exist\" does not exist");
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

TEST("Level filtering Configuration") {
  LOG::INIT { }
    .filter().levels({{DEBUG, WARN}})["stdout"]
    .filter().levels({ERROR})["stderr"];

  LOG::print {DEBUG}("Some log");
  LOG::print {INFO}("Some log");
  LOG::print {WARN}("Some log");
  LOG::print {ERROR}("Some log");
  LOG::stacktrace {ERROR};
  LOG::stacktrace {DEBUG, 5};
  LOG::stacktrace {WARN};
  return 0;
}

