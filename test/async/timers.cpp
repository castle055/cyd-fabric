// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cassert>
#include "async.h"

import std;
import fabric.logging;
using namespace fabric;
using namespace std::chrono_literals;

fabric::async::async_bus_t bus{};

void setup() {
  LOG::INIT{}.filter()["stdout"];
}

TEST("Basic Timer") {
  int  t_count = 0;
  auto t       = bus.create_timer({20ms, true}, [&] -> task<> {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
    co_return;
  });
  std::this_thread::sleep_for(70ms);
  LOG::print{INFO}("Count: {}", t_count);
  assert(t_count == 3);
  return 0;
}

TEST("Run-now Timer") {
  int  t_count = 0;
  auto t       = bus.create_timer({20ms, true, true}, [&] -> task<> {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
    co_return;
  });

  std::this_thread::sleep_for(70ms);
  LOG::print{INFO}("Count: {}", t_count);
  assert(t_count == 4);
  return 0;
}

TEST("Cancelled Timer") {
  int  t_count = 0;
  auto t       = bus.create_timer({50ms}, [&] -> task<> {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
    co_return;
  });

  std::this_thread::sleep_for(20ms);
  t.cancel();
  std::this_thread::sleep_for(50ms);
  LOG::print{INFO}("Count: {}", t_count);
  assert(t_count == 0);
  return 0;
}

TEST("Cancelled Completed Timer") {
  int  t_count = 0;
  auto t       = bus.create_timer({20ms}, [&] -> task<> {
    std::cout << "Hello from timer!" << std::endl;
    ++t_count;
    co_return;
  });

  std::this_thread::sleep_for(50ms);
  t.cancel();
  assert(t_count == 1);
  return 0;
}

TEST("Slow Timer") {
  int  t_count = 0;
  auto t       = bus.create_timer({20ms, true, true}, [&] -> task<> {
    std::cout << "Hello from...";
    std::this_thread::sleep_for(30ms);
    std::cout << " timer!" << std::endl;
    ++t_count;
    co_return;
  });

  std::this_thread::sleep_for(70ms);
  t.cancel();
  std::this_thread::sleep_for(50ms);
  LOG::print{INFO}("Count: {}", t_count);
  assert(t_count == 3);
  return 0;
}

TEST("Timer Order") {
  int  t_count = 0;
  auto t1      = bus.create_timer({50ms, true}, [&] -> task<> {
    std::cout << "Hello from timer 1!" << std::endl;
    ++t_count;
    co_return;
  });

  auto t2 = bus.create_timer({20ms}, [&] -> task<> {
    std::cout << "Hello from timer 2!" << std::endl;
    t1.cancel();
    co_return;
  });

  std::this_thread::sleep_for(70ms);
  assert(t_count == 0);
  return 0;
}
