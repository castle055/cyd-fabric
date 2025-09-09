// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ServiceContext.cppm
 *! \brief
 *!
 */

export module fabric.services:ServiceContext;
import :ServiceBase;
import :ServiceRegistry;

import std;
import reflect;

import fabric.logging;

export import fabric.tasks;
export import fabric.result;
export import fabric.exception;


namespace fabric::services {
  export struct ServiceContextOptions {
    std::string context_name{"Unnamed Context"};
  };

  export class ServiceContext {
    std::weak_ptr<ServiceContext>                              self_;
    ServiceContextOptions                                      options_;
    fabric::tasks::executor::sptr                              executor_;
    std::shared_ptr<ServiceContext>                            parent_;
    ScopeData                                                  scope_;
    ServiceRegistry::sptr                                      service_registry_;
    std::unordered_set<ServiceBase::id_type>                   starting_services_{};
    std::unordered_map<ServiceBase::id_type, ServiceBuilder>   service_factories_{};
    std::list<std::pair<ServiceBase::id_type, ServiceBuilder>> running_services_{};
    std::stop_source                                           stop_source_{};


    explicit ServiceContext(
      const fabric::tasks::executor::sptr&   executor,
      const std::shared_ptr<ServiceContext>& parent,
      const ScopeData&                       scope,
      const ServiceContextOptions&           opts
    )
        : options_(opts),
          executor_(executor),
          parent_(parent),
          scope_(scope),
          service_registry_(ServiceRegistry::make()) {
      executor_
        ->schedule([&] -> fabric::task<> {
          co_await this->make_services(stop_source_.get_token());
        }())
        .detach();
    }

  public:
    using sptr = std::shared_ptr<ServiceContext>;

    friend class ServiceLocator;

    template <ScopeConcept Scope = CurrentScope>
    static sptr make(
      const fabric::tasks::executor::sptr& executor,
      const sptr&                          parent,
      ServiceContextOptions                options = {}
    ) {
      auto ptr   = sptr(new ServiceContext(executor, parent, ScopeData::from<Scope>(), options));
      ptr->self_ = ptr;
      return std::move(ptr);
    }

    template <ScopeConcept Scope = CurrentScope>
    static sptr
    make(const fabric::tasks::executor::sptr& executor, ServiceContextOptions options = {}) {
      return make<Scope>(executor, nullptr, options);
    }

    template <ScopeConcept Scope = CurrentScope>
    static fabric::task<sptr> make(const sptr& parent, ServiceContextOptions options = {}) {
      co_return make<Scope>(co_await fabric::this_task::get_executor_sptr(), parent, options);
    }

    template <ScopeConcept Scope = CurrentScope>
    static fabric::task<sptr> make(ServiceContextOptions options = {}) {
      co_return make<Scope>(co_await fabric::this_task::get_executor_sptr(), options);
    }

    ~ServiceContext() {
      if (running_services_.empty()) {
        LOG::print{
          INFO
        }("({}) Shutting down (no remaining running services)", options_.context_name);
      } else {
        LOG::print{
          INFO
        }("({}) Shutting down (stopping {} remaining services asynchronously)",
          options_.context_name,
          running_services_.size());
      }
      stop_source_.request_stop();
      stop_all_services();
    }

    template <
      AbstractServiceConcept ServiceType,
      ServiceConcept         Implementation = ServiceType,
      typename... Args>
      requires std::same_as<ServiceType, Implementation> or
               std::is_base_of_v<ServiceType, Implementation>
    fabric::task<> configure_service(Args&&... args) {
      co_await fabric::this_task::switch_executor(executor_);

      sptr target_ctx = find_context_by_scope(scope_id<typename Implementation::scope>);
      if (target_ctx == nullptr) {
        log_context_chain();
        LOG::print{
          ERROR
        }("Cannot configure {:?} in current context chain (missing scope: {:?})",
          service_name<Implementation>,
          scope_name<typename Implementation::scope>);
        co_return;
      }

      co_await fabric::this_task::switch_executor(target_ctx->executor_);

      if (target_ctx->service_factories_.contains(service_id<ServiceType>)) {
        LOG::print{WARN}("Overriding service configuration: {:?}", service_name<ServiceType>);
      }
      target_ctx->service_factories_[service_id<ServiceType>] =
        ServiceBuilder::from<ServiceType, Implementation>(std::forward<Args>(args)...);
    }

    template <
      AbstractServiceConcept ServiceType,
      ServiceConcept         Implementation = ServiceType,
      typename... Args>
      requires std::same_as<ServiceType, Implementation> or
               std::is_base_of_v<ServiceType, Implementation>
    fabric::task<> register_service(Args&&... args) {
      co_await fabric::this_task::switch_executor(executor_);
      if (service_registry_->has_service<ServiceType>() or
          starting_services_.contains(service_id<ServiceType>)) {
        if constexpr (std::same_as<ServiceType, Implementation>) {
          LOG::print{ERROR} //
          ("({}) Service already registered: {:?}",
           options_.context_name,
           service_name<ServiceType>);
        } else {
          LOG::print{ERROR} //
          ("({}) Service already registered: {:?} implemented as {:?}",
           options_.context_name,
           service_name<ServiceType>,
           service_name<Implementation>);
        }
        co_return;
      }

      if constexpr (std::same_as<ServiceType, Implementation>) {
        LOG::print{INFO} //
        ("({}) Registering service: {:?}", options_.context_name, refl::type_name<ServiceType>);
      } else {
        LOG::print{INFO} //
        ("({}) Registering service: {:?} (implemented as {:?})",
         options_.context_name,
         service_name<ServiceType>,
         service_name<Implementation>);
      }
      starting_services_.insert(service_id<ServiceType>);
      fabric::launch(
        start_service(
          ServiceBuilder::from<ServiceType, Implementation>(std::forward<Args>(args)...)
        )
      )
        .detach();
      if constexpr (std::same_as<ServiceType, Implementation>) {
        LOG::print{DEBUG}("Start task scheduled for service: {:?}", service_name<ServiceType>);
      } else {
        LOG::print{DEBUG} //
        ("Start task scheduled for service: {:?} implemented as {:?}",
         service_name<ServiceType>,
         service_name<Implementation>);
      }
    }


    std::shared_ptr<ServiceLocator> make_locator(std::string name = "UnnamedLocator");

    void log_context_chain() const {
      LOG::print{INFO}("Context chain from ({})", options_.context_name);
      std::vector<const ServiceContext*> contexts{};
      const ServiceContext*              current_context = this;
      while (nullptr != current_context) {
        contexts.emplace_back(current_context);
        current_context = current_context->parent_.get();
      }

      std::size_t i = 0;
      for (const auto& context: std::views::reverse(contexts)) {
        if (context->scope_.id == scope_id<CurrentScope>) {
          LOG::print{INFO}("  {}: {}", i++, context->options_.context_name);
        } else {
          LOG::print{
            INFO
          }("  {}: {} (provides: {})", i++, context->options_.context_name, context->scope_.name);
        }

        std::size_t j = 0;
        for (const auto& [id, builder]: context->running_services_) {
          LOG::print{INFO}("    {}: {}", j++, builder.name);
        }
      }

      LOG::print{INFO}("Found {} contexts in context chain", contexts.size());
    }

    template <AbstractServiceConcept ServiceType>
    std::optional<std::shared_ptr<ServiceType>> find() {
      if (service_registry_->has_service<ServiceType>()) {
        return service_registry_->get_service<ServiceType>();
      }
      if (parent_ != nullptr) {
        return parent_->find<ServiceType>();
      }
      return std::nullopt;
    }

    template <AbstractServiceConcept ServiceType>
    task<ServiceType&> require(
      const char*         file_name = normalize(__builtin_FILE(), __FILE__),
      const char*         fun       = __builtin_FUNCTION(),
      const unsigned long line      = __builtin_LINE()
    );

    sptr find_context_by_scope(ScopeTag::id_type id) {
      if (id == scope_id<CurrentScope> or scope_.id == id) {
        return self_.lock();
      }

      sptr current_context = parent_;
      while (nullptr != current_context) {
        if (current_context->scope_.id == id) {
          return current_context;
        }
        current_context = current_context->parent_;
      }
      return nullptr;
    }

    template <ScopeConcept S>
    sptr find_context_by_scope() {
      return find_context_by_scope(scope_id<S>);
    }

    task<bool> ready() const {
      co_await fabric::this_task::switch_executor(executor_);
      co_return service_registry_->get_missing_services().empty() and starting_services_.empty() and
        ((parent_ == nullptr) or co_await parent_->ready());
    }

    task<> await_ready() const {
      co_await fabric::this_task::switch_executor(executor_);
      if (co_await ready()) {
        co_return;
      }
      while (co_await service_registry_->await_service_needed_or_started() != 0 and
             (not starting_services_.empty())) {
      };
    }

    template <AbstractServiceConcept ServiceType>
    task<> await_ready() const {
      co_await fabric::this_task::switch_executor(executor_);
      while (not service_registry_->has_service<ServiceType>()) {
        co_await service_registry_->await_service_needed_or_started();
      }
    }

    task<> stop_all();

  private:
    fabric::task<> make_services(std::stop_token stop_token) {
      LOG::print{INFO}("({}) Starting services", options_.context_name);

      while (not stop_token.stop_requested()) {
        // LOG::print{DEBUG}("Pending services: {}", starting_services_.size());
        if (co_await service_registry_->await_service_needed_or_started() == 0) {
          // LOG::print{INFO}("({}) All services started", options_.context_name);
          // break;
          continue;
        }

        auto missing_services = service_registry_->get_missing_services();
        service_registry_->clear_missing_services();

        for (auto& [service_id, builder]: missing_services) {
          if (not starting_services_.contains(service_id)) {
            LOG::print{DEBUG}("Starting missing service: {:?}", builder.name);
            starting_services_.insert(service_id);
            fabric::launch(start_service(builder)).detach();
            LOG::print{DEBUG}("Start task scheduled for missing service: {:?}", builder.name);
          }
        }
      }
    }

    fabric::task<> start_service(ServiceBuilder builder);

    void stop_all_services();
  };
} // namespace fabric::services
