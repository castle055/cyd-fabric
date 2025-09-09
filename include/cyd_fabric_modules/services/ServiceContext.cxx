// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ServiceContext.cxx
 *! \brief
 *!
 */

module fabric.services;

import std;
import reflect;

import fabric.logging;

import fabric.tasks;
import fabric.result;
import fabric.exception;


using namespace fabric;
using namespace fabric::services;

ServiceLocator::sptr ServiceContext::make_locator(std::string name) {
  return ServiceLocator::make(name, *this);
}


fabric::task<> ServiceContext::start_service(ServiceBuilder builder) {
  LOG::print{INFO}("({}) Starting service: {:?}", options_.context_name, builder.name);
  ServiceLocator::sptr          locator       = make_locator(builder.name);
  fabric::tasks::time_point     t0            = fabric::tasks::clock::now();
  ServiceBase::sptr             service       = co_await builder.factory(locator);
  fabric::tasks::time_point     t1            = fabric::tasks::clock::now();
  std::chrono::duration<double> elapsed_total = t1 - t0;
  std::chrono::duration<double> elapsed_self  = elapsed_total - locator->get_time_awaiting_deps();
  co_await service_registry_->register_service(builder.id, service, builder.name);
  starting_services_.erase(builder.id);
  running_services_.emplace_back(builder.id, builder);
  LOG::print{
    INFO
  }("({}) Service started: {} (total: {}, self: {}, deps: {})",
    options_.context_name,
    builder.name,
    elapsed_total,
    elapsed_self,
    locator->get_time_awaiting_deps());
}

task<> stop_all_services_task(
  ScopeData                                                  scope,
  ServiceContextOptions                                      options,
  ServiceRegistry::sptr                                      registry,
  std::list<std::pair<ServiceBase::id_type, ServiceBuilder>> running_services
) {
  auto ka_token = co_await fabric::this_task::keep_alive();
  for (auto [id, builder]: std::views::reverse(running_services)) {
    LOG::print{INFO}("({}) Stopping service: {:?}", options.context_name, builder.name);

    fabric::tasks::time_point t0  = fabric::tasks::clock::now();
    auto                      ptr = registry->get_service(id);
    co_await registry->unregister_service(id);
    co_await builder.destructor(*ptr);
    fabric::tasks::time_point t1 = fabric::tasks::clock::now();
    ptr.reset();
    fabric::tasks::time_point t2 = fabric::tasks::clock::now();

    std::chrono::duration<double> elapsed_total      = t2 - t0;
    std::chrono::duration<double> elapsed_stop       = t1 - t0;
    std::chrono::duration<double> elapsed_destructor = t2 - t1;

    LOG::print{
      INFO
    }("({}) Service stopped: {:?} (total: {}, {}::stop(): {}, ~{}(): {})",
      options.context_name,
      builder.name,
      elapsed_total,
      builder.name,
      elapsed_stop,
      builder.name,
      elapsed_destructor);
  }
}

void ServiceContext::stop_all_services() {
  if (not running_services_.empty()) {
    executor_->schedule(
      stop_all_services_task(scope_, options_, service_registry_, std::move(running_services_))
    ).detach();
    running_services_ = {};
  }
}

task<> ServiceContext::stop_all() {
  co_await fabric::this_task::switch_executor(executor_);
  LOG::print{INFO}("({}) Stopping all services", options_.context_name);
  std::size_t count = running_services_.size();
  co_await stop_all_services_task(
    scope_, options_, service_registry_, std::move(running_services_)
  );
  running_services_ = {};
  LOG::print{INFO}("({}) Stopped {} services successfully", options_.context_name, count);
}
