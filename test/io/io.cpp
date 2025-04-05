// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cassert>
#include "common.h"

import fabric.main;
import fabric.io.file;
using namespace fabric::tasks;

void setup() {}

TEST("Basic IO") {
  LOG::INIT{}.log_everything();
  return fabric::main_executor::entrypoint([] -> fabric::task<int> {
    auto file_exp =
      co_await fabric::io::file::open("/home/castle/repos/cpr/cyd-fabric/test/io/test.txt");
    fabric::io::file file = file_exp.unwrap();
    std::string      s;
    while ((co_await file.get_line(s)).unwrap()) {
      LOG::print{INFO}("{:?}", s);
    }
    co_return 0;
  });
}
