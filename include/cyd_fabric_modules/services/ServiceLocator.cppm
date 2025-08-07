// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ServiceLocator.cppm
 *! \brief
 *!
 */

export module fabric.services:ServiceLocator;
import :ServiceBase;
import :ServiceRegistry;
import :ServiceContext;

import std;
import reflect;

import fabric.logging;

export import fabric.tasks;
export import fabric.result;
export import fabric.exception;


namespace fabric::services {
  export class ServiceLocator {
    std::string                                    name_;
    ServiceContext&                                context_;
    std::shared_ptr<std::chrono::duration<double>> elapsed_awaiting_deps_{
      std::make_shared<std::chrono::duration<double>>(0)
    };

    explicit ServiceLocator(const std::string& locator_name, ServiceContext& registry)
        : name_(locator_name),
          context_(registry) {}

  public:
    using sptr = std::shared_ptr<ServiceLocator>;

    ServiceLocator(const ServiceLocator&)            = delete;
    ServiceLocator(ServiceLocator&&)                 = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
    ServiceLocator& operator=(ServiceLocator&&)      = delete;

    static sptr make(const std::string& locator_name, ServiceContext& registry) {
      return sptr(new ServiceLocator(locator_name, registry));
    }

    template <AbstractServiceConcept T>
    std::optional<std::shared_ptr<T>> find() {
      ServiceContext* current_context = &context_;
      while (current_context != nullptr) {
        if (current_context->service_registry_->has_service<T>()) {
          return current_context->service_registry_->get_service<T>();
        }
        current_context = current_context->parent_.get();
      }
      return std::nullopt;
    }

    template <AbstractServiceConcept ServiceType>
    fabric::task<ServiceType&> require() {
      if (auto service = find<ServiceType>(); service.has_value()) {
        LOG::print{INFO}("Found: {:?} (required by {:?})", refl::type_name<ServiceType>, name_);
        co_return *service.value();
      }

      ServiceContext::sptr ctx =
        context_.find_context_by_scope(scope_id<typename ServiceType::scope>);
      if (ctx == nullptr) {
        context_.log_context_chain();
        throw fabric::exception(
          std::format(
            "Cannot configure {:?} in current context chain (missing scope: {:?})",
            service_name<ServiceType>,
            scope_name<typename ServiceType::scope>
          )
        );
      }

      LOG::print{INFO}("Waiting for: {:?} (required by {:?})", refl::type_name<ServiceType>, name_);
      co_await fabric::this_task::switch_executor(ctx->executor_);

      fabric::tasks::time_point    t0 = fabric::tasks::clock::now();
      std::shared_ptr<ServiceType> service{nullptr};
      if (ctx->service_factories_.contains(service_id<ServiceType>)) {
        service = co_await ctx->service_registry_->await_service_available<ServiceType>(
          ctx->service_factories_.at(service_id<ServiceType>)
        );
      } else if constexpr (DefaultConstructibleServiceConcept<ServiceType>) {
        service = co_await ctx->service_registry_->await_service_available<ServiceType>(
          ServiceBuilder::from<ServiceType, ServiceType>()
        );
      } else {
        throw fabric::exception(
          std::format("Service not constructible: ({}) missing factory", service_name<ServiceType>)
        );
      }
      fabric::tasks::time_point t1  = fabric::tasks::clock::now();
      (*elapsed_awaiting_deps_)    += t1 - t0;

      co_return *service;
    }

    template <AbstractServiceConcept T>
    bool has() const {
      return context_.service_registry_->has_service<T>() or
             context_.find_context_by_scope(scope_id<T>)
               ->service_registry_->template has_service<T>();
    }

    std::chrono::duration<double> get_time_awaiting_deps() const {
      return *elapsed_awaiting_deps_;
    }
  };
} // namespace fabric::services
