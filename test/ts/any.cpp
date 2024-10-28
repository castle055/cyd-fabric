// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "common.h"

import fabric.ts.any;
import fabric.logging;

void setup() {
  LOG::INIT {}
    .TARGETS(
      LOG::TARGETS::STDOUT::BUILDER {}
      .entry_format("[include/cyd_fabric_modules/{entry:path}:{entry:line}][{entry:function}] {entry:message}")
      )
    .FILTERS({
      {".*", "stdout"},
    });
}

TEST("Assert Type") {
  any a = make_any<int>(23);
  any s = make_any<std::string>("asdf");

  a->assert_type<int>();
  s->assert_type<std::string>();

  return 0;
}

struct asd {
  asd& operator,(int a) {
    return *this;
  }
};

void fds(asd& a) {

}

void safasdf() {
  asd a{};
  fds((a,2,2,23));
}