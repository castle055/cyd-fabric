// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cassert>
#include "common.h"

import fabric.async;
using namespace fabric::async;
using namespace fabric::tasks;
using namespace fabric;

executor::sptr exec = executor::make();

void setup() {}


task<int> test_coroutine(const int n = 0) {
  std::println("Hello coroutine world!");
  std::flush(std::cout);
  // co_await sleep(1ms);
  co_await 1ms;
  std::println("Bye coroutine world!");
  std::flush(std::cout);
  co_return n * 2;
}

task<int> ping() {
  std::println("PING");
  co_await fabric::this_task::sleep(1ms);
  std::flush(std::cout);
  co_return 0;
}

task<> basic_coroutine() {
  co_return;
}

TEST("Basic Execution") {
  auto t = exec->schedule(basic_coroutine);
  if (t.wait_for(100ms) == std::future_status::ready) {
    (void)t.get();
    return 0;
  }
  return 1;
}

TEST("Basic Pre-instanced Execution") {
  auto t = basic_coroutine();
  exec->schedule(t);
  if (t.wait_for(100ms) == std::future_status::ready) {
    (void)t.get();
    return 0;
  }
  return 1;
}

TEST("Basic Captureless Lambda Execution") {
  auto t = exec->schedule([] -> task<> { co_return; });
  if (t.wait_for(100ms) == std::future_status::ready) {
    (void)t.get();
    return 0;
  }
  return 1;
}

TEST("Basic Capture Lambda Execution") {
  int  some_value = 1234;
  auto t          = exec->schedule([some_value] -> task<> {
    assert(1234 == some_value);
    co_return;
  });
  if (t.wait_for(100ms) == std::future_status::ready) {
    (void)t.get();
    return 0;
  }
  return 1;
}

TEST("Basic Return") {
  auto t = exec->schedule([] -> task<int> { co_return 1234; });
  t.wait();
  assert(1234 == t.get());
  return 0;
}

TEST("Basic Parameter Passing") {
  auto t = exec->schedule([](int val) -> task<int> { co_return val; }, 1234);
  t.wait();
  assert(1234 == t.get());
  return 0;
}

TEST("Basic Default Parameter") {
  auto t = exec->schedule([](int val = 1234) -> task<int> { co_return val; });
  t.wait();
  assert(1234 == t.get());
  return 0;
}

TEST("Basic LRef Parameter") {
  int  val = 0;
  auto t   = exec->schedule(
    [](int& val) -> task<int> {
      val = 1234;
      co_return val;
    },
    val
  );

  t.wait();
  assert(1234 == val);
  assert(1234 == t.get());
  assert(val == t.get());
  return 0;
}

TEST("Basic RRef Parameter") {
  auto t = exec->schedule([](int&& val) -> task<int> { co_return val; }, 1234);
  t.wait();
  assert(1234 == t.get());
  return 0;
}

TEST("Basic co_await") {
  auto t = exec->schedule([] -> task<> { co_await basic_coroutine(); });
  if (t.wait_for(100ms) == std::future_status::ready) {
    (void)t.get();
    return 0;
  }
  return 1;
}

TEST("Captureless Lambda co_await") {
  auto t = exec->schedule([] -> task<> { co_await [] -> task<> { co_return; }(); });
  if (t.wait_for(100ms) == std::future_status::ready) {
    (void)t.get();
    return 0;
  }
  return 1;
}

TEST("Capture Lambda co_await") {
  auto t = exec->schedule([] -> task<int> {
    int val = 0;
    co_await [&] -> task<> {
      val = 1234;
      co_return;
    }();
    co_return val;
  });
  if (t.wait_for(100ms) == std::future_status::ready) {
    assert(1234 == t.get());
    return 0;
  }
  return 1;
}

TEST("Multiple Successive co_await") {
  auto t = exec->schedule([] -> task<int> {
    int val = 0;
    co_await [&] -> task<> {
      val = 1;
      co_return;
    }();
    assert(1 == val);
    val = co_await [&] -> task<int> { co_return val + 1; }();
    assert(2 == val);
    val = co_await [](int v) -> task<int> { co_return v + 1; }(val);
    assert(3 == val);
    val = co_await []() -> task<int> { co_return 1234; }();
    co_return val;
  });
  if (t.wait_for(100ms) == std::future_status::ready) {
    assert(1234 == t.get());
    return 0;
  }
  return 1;
}

TEST("Basic Exception Handling") {
  auto t = exec->schedule([] -> task<> {
    throw std::runtime_error("Test exception");
    co_return;
  });
  if (t.wait_for(100ms) == std::future_status::ready) {
    try {
      t.get();
      return 2; // No exception was thrown
    } catch (std::exception& e) {
      assert(std::string{"Test exception"} == std::string{e.what()});
    }
    return 0;
  }
  return 1;
}

TEST("co_await Exception Handling") {
  auto thrower = [] -> task<> {
    throw std::runtime_error("Test exception");
    co_return;
  };
  auto t = exec->schedule([&] -> task<bool> {
    try {
      co_await thrower();
      co_return false; // No exception was thrown
    } catch (std::exception& e) {
      assert(std::string{"Test exception"} == std::string{e.what()});
    }
    co_return true;
  });
  if (t.wait_for(100ms) == std::future_status::ready) {
    const bool res = t.get();
    assert(res);
    return 0;
  }
  return 1;
}

TEST("co_await Exception Propagation") {
  auto thrower = [] -> task<> {
    throw std::runtime_error("Test exception");
    co_return;
  };
  auto t = exec->schedule([&] -> task<> {
    co_await thrower();
    co_return;
  });
  if (t.wait_for(100ms) == std::future_status::ready) {
    try {
      t.get();
      return 2; // No exception was thrown
    } catch (std::exception& e) {
      assert(std::string{"Test exception"} == std::string{e.what()});
    }
    return 0;
  }
  return 1;
}

TEST("Nominal") {
  auto h = test_coroutine(2);
  // h.get_handle().resume();
  exec->schedule(h);
  exec->schedule(ping);
  exec->schedule(ping);

  h.wait();

  assert(h.get() == 4);

  return 0;
}

TEST("Lambda coroutine") {
  auto t = exec->schedule([&] -> task<> {
    co_await test_coroutine(2);
    co_return;
  });

  t.wait();
  return 0;
}
