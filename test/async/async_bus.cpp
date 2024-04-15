// Copyright (c) 2024, Victor Castillo, All rights reserved.

#include "async.h"
#include <cassert>

#include <chrono>

std::unique_ptr<cydui::async::async_bus_t> bus = test::async::make_async_bus();

void setup() {
}

TEST("Nominal Start-up") (
  assert(bus->status == cydui::async::async_bus_status_e::RUNNING);
  assert(bus->thread != nullptr); return 0;
)

TEST("Nominal Termination") (
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
  assert(bus->status == cydui::async::async_bus_status_e::STOPPED);
  assert(bus->thread == nullptr);
  return 0;
)

TEST("Nominal Restart") (
  bus->thread_stop();
  assert(bus->status == cydui::async::async_bus_status_e::STOPPED);
  assert(bus->thread == nullptr);
  bus->thread_start();
  assert(bus->status == cydui::async::async_bus_status_e::RUNNING);
  assert(bus->thread != nullptr);
  return 0;
)
