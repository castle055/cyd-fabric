// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "async.h"
#include <cassert>

#include <chrono>

std::unique_ptr<cyd::fabric::async::async_bus_t> bus = test::async::make_async_bus();

void setup() {
}

TEST("Nominal Start-up") {
  assert(bus->status == cyd::fabric::async::async_bus_status_e::RUNNING);
  assert(bus->thread != nullptr); return 0;
}

TEST("Nominal Termination") {
  auto t0 = std::chrono::system_clock::now();
  bus->thread_stop();
  auto t1 = std::chrono::system_clock::now();
  /// Can't take to long to join this thread
  std::cout
    << "Took "
    << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0)
    << " to join the bus thread."
    << std::endl;
  //assert((t1 - t0) < 300us);
  assert(bus->status == cyd::fabric::async::async_bus_status_e::STOPPED);
  assert(bus->thread == nullptr);
  return 0;
}

TEST("Nominal Restart") {
  bus->thread_stop();
  assert(bus->status == cyd::fabric::async::async_bus_status_e::STOPPED);
  assert(bus->thread == nullptr);
  bus->thread_start();
  assert(bus->status == cyd::fabric::async::async_bus_status_e::RUNNING);
  assert(bus->thread != nullptr);
  return 0;
}
