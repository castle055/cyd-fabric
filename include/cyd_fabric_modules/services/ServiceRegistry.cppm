// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ServiceRegistry.cppm
 *! \brief
 *!
 */

export module fabric.services:ServiceRegistry;
import :ServiceBase;

import std;
import reflect;

import fabric.logging;

export import fabric.tasks;
export import fabric.result;
export import fabric.exception;


namespace fabric::services {
  export class ServiceRegistry {
    std::unordered_map<ServiceBase::id_type, ServiceSlot> services_{};
    std::unordered_map<ServiceBase::id_type, std::list<tasks::task_handle<>>>
                                                             service_awaiting_tasks_{};
    std::unordered_map<ServiceBase::id_type, ServiceBuilder> missing_services_{};
    std::list<tasks::task_handle<>>                          service_needed_awaiting_tasks_{};

    ServiceRegistry();

    struct awaitable_service_available;
    struct awaitable_service_needed;


    task<> register_service(ServiceBase::id_type id, const ServiceSlot& service);

  public:
    using sptr = std::shared_ptr<ServiceRegistry>;

    static sptr make();

    task<> register_service(
      ServiceBase::id_type id, const ServiceBase::sptr& service, const std::string name
    );
    task<>            unregister_service(ServiceBase::id_type id);
    bool              has_service(ServiceBase::id_type id) const;
    ServiceBase::sptr get_service(ServiceBase::id_type id);

    const std::unordered_map<ServiceBase::id_type, ServiceBuilder>& get_missing_services();
    void                                                            clear_missing_services();

    fabric::task<std::size_t> await_service_needed_or_started();
    fabric::task<ServiceBase::sptr>
    await_service_available(ServiceBase::id_type id, ServiceBuilder default_builder);


    template <AbstractServiceConcept T>
    task<> register_service(const ServiceBase::sptr& service) {
      if (services_.contains(service_id<T>)) {
        throw fabric::exception{
          std::format("{} with ID '{}' already registered", refl::type_name<T>, service_id<T>)
        };
      }
      co_await register_service(
        service_id<T>, ServiceSlot{service_id<T>, service, std::string{refl::type_name<T>}}
      );
    }

    template <AbstractServiceConcept T>
    bool has_service() const {
      return has_service(service_id<T>);
    }

    template <AbstractServiceConcept T>
    std::shared_ptr<T> get_service() {
      if (not services_.contains(service_id<T>)) {
        throw fabric::exception{std::format("No such service '{}'", refl::type_name<T>)};
      }
      return std::dynamic_pointer_cast<T>(services_.at(service_id<T>).service);
    }

    template <AbstractServiceConcept T>
    fabric::task<std::shared_ptr<T>> await_service_available(ServiceBuilder default_builder) {
      co_return std::dynamic_pointer_cast<T>(
        co_await await_service_available(service_id<T>, default_builder)
      );
    }

    const std::unordered_map<ServiceBase::id_type, ServiceSlot>& get_all_services() const {
      return services_;
    }
  };
} // namespace fabric::services
