// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ServiceRegistry.cxx
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

struct ServiceRegistry::awaitable_service_available {
  ServiceRegistry&     registry;
  ServiceBase::id_type service_id;
  ServiceBuilder       builder;

  bool await_ready() const noexcept {
    return false;
  }

  template <typename P>
  void await_suspend(tasks::task_handle<P> h) noexcept {
    if (not registry.service_awaiting_tasks_.contains(service_id)) {
      registry.service_awaiting_tasks_[service_id] = {};
    }
    if (not registry.missing_services_.contains(service_id)) {
      registry.missing_services_.emplace(service_id, builder);
    }
    registry.service_awaiting_tasks_.at(service_id).emplace_back(std::move(h));

    // LOG::print{DEBUG}("Waking {} tasks", registry.service_needed_awaiting_tasks_.size());
    // for (auto& service_needed_awaiting_task: registry.service_needed_awaiting_tasks_) {
    //   service_needed_awaiting_task.resume();
    // }
    // auto t = registry.service_needed_awaiting_tasks_.front();
    // registry.service_needed_awaiting_tasks_.clear();
    // return t;
    // return [](std::list<tasks::task_handle<>> awaiting_tasks) -> fabric::task<> {
    //   LOG::print{DEBUG}("Waking {} tasks", awaiting_tasks.size());
    //   for (const auto& service_needed_awaiting_task: awaiting_tasks) {
    //     co_await fabric::launch(service_needed_awaiting_task);
    //   }
    //   co_return;
    // }(std::move(registry.service_needed_awaiting_tasks_)).get_handle();
  }

  ServiceBase::sptr await_resume() const noexcept {
    return registry.get_service(service_id);
  }
};

struct ServiceRegistry::awaitable_service_needed {
  ServiceRegistry& registry;

  bool await_ready() const noexcept {
    if (not registry.missing_services_.empty()) {
      LOG::print{
        DEBUG
      }("There are {} missing services, resuming", registry.missing_services_.size());
    }
    return not registry.missing_services_.empty();
  }

  template <typename P>
  void await_suspend(tasks::task_handle<P> h) noexcept {
    LOG::print{DEBUG}("There are no missing services, suspending");
    registry.service_needed_awaiting_tasks_.emplace_back(std::move(h));
  }

  std::size_t await_resume() const noexcept {
    return registry.missing_services_.size();
  }
};

ServiceRegistry::ServiceRegistry() {}

ServiceRegistry::sptr ServiceRegistry::make() {
  return sptr(new ServiceRegistry());
}


task<> ServiceRegistry::register_service(ServiceBase::id_type id, const ServiceSlot& service) {
  services_.emplace(id, service);
  LOG::print{DEBUG}("Service registered: {:?} [ID: {}]", service.name, id);

  LOG::print{DEBUG}(
    "Waking {} awaiting tasks for service needed or started", service_needed_awaiting_tasks_.size()
  );
  co_await fabric::launch(service_needed_awaiting_tasks_);
  service_needed_awaiting_tasks_.clear();

  if (service_awaiting_tasks_.contains(id)) {
    auto tasks = std::move(service_awaiting_tasks_.at(id));
    service_awaiting_tasks_.erase(id);
    LOG::print{DEBUG}("Waking {} awaiting tasks for {:?}", tasks.size(), service.name);
    for (const auto& awaiting_task: tasks) {
      co_await fabric::launch(awaiting_task);
    }
  }
}

task<> ServiceRegistry::register_service(
  ServiceBase::id_type id, const ServiceBase::sptr& service, const std::string name
) {
  if (services_.contains(id)) {
    throw fabric::exception{std::format("{} with ID '{}' already registered", name, id)};
  }
  co_await register_service(id, ServiceSlot{id, service, name});
}

task<> ServiceRegistry::unregister_service(ServiceBase::id_type id) {
  services_.erase(id);
  co_return;
}

ServiceBase::sptr ServiceRegistry::get_service(ServiceBase::id_type id) {
  if (not services_.contains(id)) {
    throw fabric::exception{std::format("No such service with id '{}'", id)};
  }
  return services_.at(id).service;
}

bool ServiceRegistry::has_service(ServiceBase::id_type id) const {
  return services_.contains(id);
}

const std::unordered_map<ServiceBase::id_type, ServiceBuilder>&
ServiceRegistry::get_missing_services() {
  std::vector<std::pair<ServiceBase::id_type, ServiceBuilder>> missing_dependencies{};
  return missing_services_;
}

void ServiceRegistry::clear_missing_services() {
  missing_services_.clear();
}

fabric::task<std::size_t> ServiceRegistry::await_service_needed_or_started() {
  LOG::print{DEBUG}("Awaiting service needed or started");
  std::size_t missing_count = co_await awaitable_service_needed{*this};
  co_return missing_count;
}

fabric::task<ServiceBase::sptr>
ServiceRegistry::await_service_available(ServiceBase::id_type id, ServiceBuilder default_builder) {
  if (services_.contains(id)) {
    co_return services_.at(id).service;
  }

  LOG::print{DEBUG}(
    "Waking {} awaiting tasks for service needed or started", service_needed_awaiting_tasks_.size()
  );
  co_await fabric::launch(service_needed_awaiting_tasks_);
  service_needed_awaiting_tasks_.clear();

  co_return co_await awaitable_service_available{*this, id, default_builder};
}
