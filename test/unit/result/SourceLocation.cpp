// Copyright (c) 2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"

import fabric.source_location;
import fabric.logging;

int some_fun(const fabric::SourceLocation& sloc = {}) {
  std::println("{}", sloc.to_string());
  return 0;
}

int a = some_fun();

struct some_struct {
  int a = some_fun();
  int b;
  int c;
  some_struct(): b(some_fun()) {
    c = some_fun();
  }
};

some_struct some_obj{};

TEST(SourceLocation, Test) {
  some_fun();
  some_fun();
  some_fun();
  [] { some_fun(); }();
  some_struct some_obj1{};
}
