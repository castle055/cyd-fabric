// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ServiceBase.cppm
 *! \brief
 *!
 */

export module fabric.services:ServiceBase;

import std;
import reflect;

import fabric.logging;

export import fabric.tasks;
export import fabric.result;
export import fabric.exception;


namespace fabric::services {
  export struct ScopeTag {
    using id_type = refl::type_id_t;
  };

  export template <typename T>
  concept ScopeConcept = std::is_base_of_v<ScopeTag, T>;

  export template <ScopeConcept S>
  constexpr ScopeTag::id_type scope_id = refl::type_id<S>;

  export template <ScopeConcept S>
  constexpr std::string_view scope_name = refl::type_name<S>;

  export struct RootScope: ScopeTag {};
  export struct CurrentScope: ScopeTag {};

  export struct ScopeData {
    ScopeTag::id_type id;
    std::string       name;

    template <ScopeConcept Scope>
    static constexpr ScopeData from() {
      return {scope_id<Scope>, std::string{scope_name<Scope>}};
    }
  };

  export class ServiceLocator;

  export class ServiceBase {
  public:
    using sptr    = std::shared_ptr<ServiceBase>;
    using id_type = refl::type_id_t;
    using scope   = CurrentScope;

    virtual ~ServiceBase() {}

    ServiceBase()                              = default;
    ServiceBase(const ServiceBase&)            = delete;
    ServiceBase(ServiceBase&&)                 = delete;
    ServiceBase& operator=(const ServiceBase&) = delete;
    ServiceBase& operator=(ServiceBase&&)      = delete;
  };

  export template <typename T>
  concept AbstractServiceConcept = std::is_base_of_v<ServiceBase, T>;

  export template <typename T>
  concept ServiceConcept = AbstractServiceConcept<T>; // and requires { T::start; };

  export template <typename T>
  concept DefaultConstructibleServiceConcept =
    ServiceConcept<T> and fabric::is_task_v<decltype(T::start(std::declval<ServiceLocator&>()))>;

  export template <AbstractServiceConcept T>
  constexpr refl::type_id_t service_id = refl::type_id<T>;

  export template <AbstractServiceConcept S>
  constexpr std::string_view service_name = refl::type_name<S>;

  export using ServiceFactory =
    std::function<task<ServiceBase::sptr>(std::shared_ptr<ServiceLocator>)>;

  export struct ServiceBuilder {
    ServiceBase::id_type                        id;
    std::string                                 name;
    ServiceFactory                              factory;
    std::function<fabric::task<>(ServiceBase&)> destructor;

    template <
      AbstractServiceConcept ServiceType,
      ServiceConcept         Implementation = ServiceType,
      typename... Args>
      requires std::same_as<ServiceType, Implementation> or
               std::is_base_of_v<ServiceType, Implementation>
    static ServiceBuilder from(Args&&... args) {
      return ServiceBuilder{
        .id   = service_id<ServiceType>,
        .name = std::string{refl::type_name<ServiceType>},
        .factory =
          [args...](std::shared_ptr<ServiceLocator> locator) -> fabric::task<ServiceBase::sptr> {
          co_return co_await Implementation::start(*locator, args...);
        },
        .destructor = [](ServiceBase& service) -> fabric::task<> {
          if constexpr (requires { Implementation::stop; }) {
            co_await Implementation::stop(*dynamic_cast<Implementation*>(&service));
          } else {
          }
          co_return;
        }
      };
    }
  };

  export struct ServiceSlot {
    ServiceBase::id_type id;
    ServiceBase::sptr    service;
    std::string          name;
  };
} // namespace fabric::services
