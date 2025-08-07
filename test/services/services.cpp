// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cassert>
#include "common.h"

import std;
import fabric.logging;
import fabric.services;
import fabric.main;

using namespace std::chrono_literals;


void setup() {
  // LOG::INIT{}.log_everything();
  LOG::INIT{}.filter().min_level(INFO)["stdout"];
}

struct GlobalScope: fabric::services::ScopeTag {};
struct WindowScope: fabric::services::ScopeTag {};

class AService: public fabric::services::ServiceBase {
  AService() {}

public:
  using scope = GlobalScope;
  static fabric::task<sptr> start(fabric::services::ServiceLocator& service_locator) {
    co_return sptr{new AService()};
  }
};

class BService: public fabric::services::ServiceBase {
  fabric::tasks::executor::sptr executor_;

  BService(const fabric::tasks::executor::sptr& executor)
      : executor_(executor) {}

public:
  using scope = GlobalScope;

  // static fabric::task<sptr> start(fabric::services::ServiceLocator& service_locator) {
  //   LOG::print{INFO}("BService: Using default constructor");
  //   auto& a = co_await service_locator.require<AService>();
  //
  //   co_return sptr{new BService(co_await fabric::this_task::get_executor_sptr())};
  // }
  static fabric::task<sptr> start(fabric::services::ServiceLocator& service_locator, int i) {
    LOG::print{INFO}("BService: Using constructor with param i={}", i);
    auto& a = co_await service_locator.require<AService>();

    co_return sptr{new BService(co_await fabric::this_task::get_executor_sptr())};
  }

  fabric::task<> do_something() {
    co_await fabric::this_task::switch_executor(executor_);
    LOG::print{INFO}("Doing something");
    // co_await 1s;
    LOG::print{INFO}("Done something");
    co_return;
  }
};

class CService: public fabric::services::ServiceBase {
  CService() = default;

public:
  using scope = WindowScope;
  static fabric::task<sptr> start(fabric::services::ServiceLocator& service_locator, int i) {
    LOG::print{INFO}("CService: Using constructor with param i={}", i);
    auto& b = co_await service_locator.require<BService>();
    auto& a = co_await service_locator.require<AService>();

    LOG::print{INFO}("Doing something");
    co_await b.do_something();
    LOG::print{INFO}("Done something");

    co_return sptr{new CService()};
  }

  static fabric::task<> stop(CService& self) {
    LOG::print{INFO}("CService stopping");
    // co_await 1s;
    co_return;
  }
};

TEST("Basic Test") {
  return fabric::main([] -> fabric::task<int> {
    auto global_ctx =
      co_await fabric::services::ServiceContext::make<GlobalScope>({"GlobalContext"});
    auto exec = fabric::tasks::executor::make();
    {
      auto ctx = fabric::services::ServiceContext::make(exec, global_ctx, {"WindowContext"});
      ctx->log_context_chain();

      co_await ctx->configure_service<BService>(123);
      co_await ctx->register_service<CService>(123);

      // co_await global_ctx->make_services();
      // co_await ctx->make_services();
      co_await ctx->await_ready();
      ctx->log_context_chain();

      co_await ctx->stop_all();
      ctx->log_context_chain();
    }
    co_return 0;
  });
}
