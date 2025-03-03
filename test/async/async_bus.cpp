// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "async.h"
#include <cassert>

#include <chrono>

using namespace std::chrono_literals;

class test_bus: public fabric::async::async_bus_t {
public:
  void start() {
    async_bus_t::start();
  }

  void stop() {
    async_bus_t::stop();
  }
};

void setup() {
}

TEST("Nominal Timer") {
  test_bus bus{};
  bus.start();

  int t_count = 0;
  auto t = bus.create_timer({20ms, true}, [&] {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
  });

  std::this_thread::sleep_for(70ms);
  bus.stop();
  assert(t_count == 3);
  return 0;
}

TEST("Run-now Timer") {
  test_bus bus{};
  bus.start();

  int t_count = 0;
  auto t = bus.create_timer({20ms, true, true}, [&] {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
  });

  std::this_thread::sleep_for(70ms);
  bus.stop();
  assert(t_count == 4);
  return 0;
}

TEST("Cancelled Timer") {
  test_bus bus{};
  bus.start();

  int t_count = 0;
  auto t = bus.create_timer({50ms}, [&] {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
  });

  std::this_thread::sleep_for(20ms);
  t.cancel();
  std::this_thread::sleep_for(50ms);
  bus.stop();
  assert(t_count == 0);
  return 0;
}

TEST("Cancelled Completed Timer") {
  test_bus bus{};
  bus.start();

  int t_count = 0;
  auto t = bus.create_timer({20ms}, [&] {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
  });

  std::this_thread::sleep_for(50ms);
  t.cancel();
  bus.stop();
  assert(t_count == 1);
  return 0;
}

TEST("Too slow Timer") {
  test_bus bus{};
  bus.start();

  int t_count = 0;
  auto t = bus.create_timer({20ms, true, true}, [&] {
    std::cout << "Hello from...";
    std::this_thread::sleep_for(30ms);
    std::cout << " timer!" << std::endl;
    ++t_count;
  });

  std::this_thread::sleep_for(70ms);
  t.cancel();
  bus.stop();
  assert(t_count == 2);
  return 0;
}

TEST("Two Timers") {
  test_bus bus{};
  bus.start();

  int t_count = 0;
  auto t1 = bus.create_timer({50ms, true}, [&] {
    std::cout << "Hello from timer 1!" << std::endl;
    ++t_count;
  });

  auto t2 = bus.create_timer({20ms}, [&] {
    std::cout << "Hello from timer 2!" << std::endl;
    t1.cancel();
  });

  std::this_thread::sleep_for(70ms);
  bus.stop();
  assert(t_count == 0);
  return 0;
}

//
// TEST("Nominal Termination") {
//   auto t0 = std::chrono::system_clock::now();
//   bus->thread_stop();
//   auto t1 = std::chrono::system_clock::now();
//   /// Can't take to long to join this thread
//   std::cout
//     << "Took "
//     << std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0)
//     << " to join the bus thread."
//     << std::endl;
//   //assert((t1 - t0) < 300us);
//   assert(bus->status == fabric::async::async_bus_status_e::STOPPED);
//   assert(bus->thread == nullptr);
//   return 0;
// }
//
// TEST("Nominal Restart") {
//   bus->thread_stop();
//   assert(bus->status == fabric::async::async_bus_status_e::STOPPED);
//   assert(bus->thread == nullptr);
//   bus->thread_start();
//   assert(bus->status == fabric::async::async_bus_status_e::RUNNING);
//   assert(bus->thread != nullptr);
//   return 0;
// }
