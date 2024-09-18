// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

// TODO - Delete this file

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async.ebus2;

import fabric.async.ebus;


struct TestEvent {
  static constexpr auto type = "TestEvent";
  int a {2};
};

struct OtherEvent {
  static constexpr auto type = "OtherEvent";
};

void asdfasdfasd() {
  auto ebus = fabric::async::make_ebus();
  ebus.make_producer().emit<TestEvent>(2);
  ebus.make_consumer().on_event([](const TestEvent &ev) {});

  ebus->emit<TestEvent>(2);
  ebus->emit<TestEvent>();
  ebus->emit<OtherEvent>();

  auto listener = ebus->on_event([](const OtherEvent &ev) -> int {
    // ev.a;
      return 0;
  });
  ebus->on_event<TestEvent>([](const TestEvent &ev) {
    ev.type;
  });

  listener.remove();

  const auto runner = ebus.claim_runner();

  while (runner) {
    runner.events_process_batch();
  }

  ebus->evict_runner();

}