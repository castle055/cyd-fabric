// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cassert>
#include "async.h"
#include "cyd_fabric_modules/headers/macros/async_events.h"

import std;
import reflect.serialize;
import fabric.logging;
using namespace fabric;
using namespace std::chrono_literals;

fabric::async::async_bus_t bus{};

void setup() {
}

EVENT(TestEvent) {
  int value = 1;
};

TEST("Basic Event Handling") {
  int t_count = 0;

  auto listener = bus.on_event([&](const TestEvent& ev) -> task<> {
    LOG::print{INFO}("Hello from event dispatch!");
    LOG::print{INFO}("Received Event: TestEvent {}", refl::serializer<formats::json_fmt>::to_string(ev));
    t_count += ev.value;
    co_return;
  });

  bus.emit<TestEvent>();

  std::this_thread::sleep_for(70ms);
  LOG::print{INFO}("Count: {}", t_count);
  assert(t_count == 1);
  return 0;
}
