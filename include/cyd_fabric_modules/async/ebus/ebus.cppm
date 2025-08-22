// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ebus.cppm
 *! \brief
 *!
 */

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async.ebus:impl;

import std;
export import reflect;
import reflect.serialize;

import :types;
export import :event;
export import :raw_listener;
export import :typed_listener;
export import fabric.async.scheduler;
export import fabric.tasks;
export import fabric.logging;

namespace fabric::async {
  class awaitable_events {
    std::atomic_flag            awaiting_events_{false};
    std::atomic_flag            pending_events_{false};
    fabric::task<>::handle_type handle_;

  public:
    bool await_ready() const noexcept {
      // LOG::print{DEBUG}("EBus ready: {}", pending_events_.test());
      return pending_events_.test();
    } /// Always suspend!

    void await_suspend(task<>::handle_type h) noexcept {
      awaiting_events_.test_and_set();
      handle_ = h;
      // LOG::print{DEBUG}("EBus suspended");
    }

    void await_resume() noexcept {
      pending_events_.clear();
    }

    void notify() noexcept {
      if (awaiting_events_.test()) {
        awaiting_events_.clear();
        handle_.promise().reschedule();
      } else {
        pending_events_.test_and_set();
      }
      // LOG::print{DEBUG}("EBus notified");
    }
  };
} // namespace fabric::async

export namespace fabric::async {
  class ebus {
  public:
    using wptr = std::weak_ptr<ebus>;
    struct sptr: std::shared_ptr<ebus> {
      explicit sptr(ebus* ev_queue)
          : shared_ptr<fabric::async::ebus>(ev_queue) {}

      ebus_producer make_producer() const;

      ebus_consumer make_consumer() const;

      ebus_actor make_actor() const;

      ebus_runner claim_runner() const;
    };

    friend sptr make_ebus();
    friend class ebus_producer;
    friend class ebus_consumer;
    friend class ebus_actor;
    friend class ebus_runner;

  protected:
    ebus() = default;

  private: /// @name Raw Event Handling
    // ? This function creates a copy of the event, thus increasing its ref count.
    void push_event(const fabric::async::event::sptr& ev) {
      std::unique_lock lk{event_mutex};
      front_ebus.push(ev);
      events_awaitable.notify();
    }

    event::sptr& emit_raw(event::sptr& ev) {
      push_event(ev);
      return ev;
    }

    event::sptr
    emit_raw(const std::string& event_type, void* data, std::function<void()>&& data_destructor) {
      event::sptr ev =
        make_event(event_type, data, std::forward<std::function<void()>&&>(data_destructor));
      push_event(ev);
      return ev;
    }

    raw_listener::sptr on_event_raw(const std::string& event_type, auto l_) {
      LOG::print{DEBUG}("Adding event listener: {}", event_type);
      std::unique_lock lk{listeners_mutex};
      auto             l = std::make_shared<raw_listener>(this, event_type, l_);
      event_listeners[event_type].push_back(l);

      return l;
    }

  private: /// @name Event Processing
    void swap_ebuss() {
      std::scoped_lock lk{event_mutex};
      std::swap(front_ebus, back_ebus);
    }

    std::vector<std::shared_ptr<raw_listener>> get_listeners_for_event(const std::string& ev_type) {
      std::vector<std::shared_ptr<raw_listener>> listeners{};

      if (event_listeners.contains(ev_type)) {
        const auto& ev_listeners = event_listeners.at(ev_type);
        listeners.reserve(ev_listeners.size());
        for (const auto& item: ev_listeners) {
          listeners.emplace_back(item);
        }
      }

      return listeners;
    }

    task<> process_event_task(const tasks::executor& exec, const event::sptr& ev) {
      ev->status = EventStatus::PROCESSING;
      std::vector<std::shared_ptr<raw_listener>> listeners;
      { // Make copy of listeners with the mutex
        std::unique_lock lk{listeners_mutex};
        listeners = get_listeners_for_event(ev->type);
      }
      LOG::print{DEBUG}("Calling ({}) listeners for {}", listeners.size(), ev->type);

      // Iterate over copy of listeners list. This should allow any listener to modify the listeners
      // list (ie: removing themselves)
      for (const auto& listener: listeners) {
        co_await listener->operator()(exec, ev);
      }

      // Clean up event
      ev->status = EventStatus::CONSUMED;
      co_return;
    }

    task<> process_all_events_task(const tasks::executor& exec, std::queue<event::sptr>& ev_queue) {
      LOG::print{DEBUG}("Processing ({}) events", ev_queue.size());
      while (!ev_queue.empty()) {
        co_await process_event_task(exec, ev_queue.front());
        ev_queue.pop();
      }
      co_return;
    }

  protected: /// @name Bus Interface
    task<> event_processing_task() {
      LOG::print{DEBUG}("Event processing started");
      const auto& exec = co_await this_task::get_executor();
      while (true) {
        co_await events_awaitable;
        swap_ebuss();
        co_await process_all_events_task(exec, back_ebus);
      }
      co_return;
    }

  private:
    template <EventType T>
    void log_event(const T& it) {
      LOG::print{
        DEBUG
      }("<EVENT> {}: {}", refl::type_name<T>, refl::serializer<formats::json_fmt>::to_string(it));
    }

  public: /// @name Public Interface
    template <EventType T>
    inline event::sptr emit() {
      auto* data_ptr = new T();
      log_event(*data_ptr);
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    template <EventType T>
    inline event::sptr emit(const T& event) {
      auto* data_ptr = new T(event);
      log_event(*data_ptr);
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    template <EventType T>
    inline event::sptr emit(T&& event) {
      auto* data_ptr = new T(std::forward<T&&>(event));
      log_event(*data_ptr);
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    template <EventType T, typename... EVFields>
    inline event::sptr emit(EVFields&&... fields) {
      auto* data_ptr = new T(std::forward<EVFields&&>(fields)...);
      log_event(*data_ptr);
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    inline auto on_event(auto&& c) -> listener<event_type_from_handler<decltype(c)>>
      requires(EventType<event_type_from_handler<decltype(c)>>)
    {
      using EventT = event_type_from_handler<decltype(c)>;
      return listener<EventT>{
        on_event_raw(EventT::type, [c = std::move(c)](const event& ev) -> task<> {
          co_await c(ev.as<EventT>());
          co_return;
        })
      };
    }

    template <typename T>
    inline auto on_event(auto&& c) -> listener<event_type_from_handler<decltype(c)>>
      requires(
        EventType<event_type_from_handler<decltype(c)>> and
        std::same_as<T, event_type_from_handler<decltype(c)>>
      )
    {
      return on_event(c);
    }

    void remove_listener(const raw_listener& listener) {
      if (listener.get_id() == 0)
        return;
      const std::string& event_type = listener.event_type();
      LOG::print{DEBUG}("Removing listener for event: {}", event_type);
      std::unique_lock lk{listeners_mutex};
      if (event_listeners.contains(event_type)) {
        for (auto l = event_listeners[event_type].begin();
             l != event_listeners[event_type].end();) {
          if (l->expired()) {
            l = event_listeners[event_type].erase(l);
            continue;
          }
          if (l->lock()->get_id() == listener.get_id()) {
            l = event_listeners[event_type].erase(l);
            return;
          } else {
            ++l;
          }
        }
      }
    }

    template <EventType T>
    task<T> await() {
      co_return co_await event_awaitable<T>{this};
    }

    template <EventType T>
    task<T> await(auto&& filter) {
      T ev{};
      while (ev = co_await await<T>(), not filter(ev))
        ;
      co_return ev;
    }

    bool is_runner_claimed() const {
      return runner_claimed_ != nullptr;
    }

    void evict_runner() {
      runner_claimed_ = nullptr;
    }

  public: /// @name Getter
    ebus& get_ebus() {
      return *this;
    }

  public: /// @name Destructor
    ~ebus() {
      for (auto& item: event_listeners) {
        auto listeners = get_listeners_for_event(item.first);
        for (auto& l: listeners) {
          l->remove();
        }
      }
    }

  private
    TEST_PUBLIC: std::queue<event::sptr> front_ebus{};
    std::queue<event::sptr>              back_ebus{};

    std::unordered_map<std::string, std::list<raw_listener::wptr>> event_listeners{};

  private
    TEST_PUBLIC: std::mutex event_mutex;
    std::mutex              listeners_mutex;

    awaitable_events events_awaitable{};
  private
    TEST_PUBLIC: ebus_runner* runner_claimed_ = nullptr;

    template <EventType T>
    struct event_awaitable {
      ebus* bus;
      std::optional<listener<T>> listener{std::nullopt};
      T event;

      bool await_ready() const noexcept {
        return false;
      }

      template <typename P>
      void await_suspend(std::coroutine_handle<P> h) noexcept {
        listener = bus->on_event<T>([=, this](const T& ev) -> fabric::task<> {
          event = ev;
          h.promise().reschedule();
          co_return;
        });
      }

      T await_resume() noexcept {
        return event;
      }
    };
  };

  void raw_listener::remove() {
    if (nullptr != ebus_)
      ebus_->remove_listener(*this);
    delete func_;
    func_  = nullptr;
    active = false;
  }

  ebus::sptr make_ebus() {
    return ebus::sptr(new ebus());
  }
} // namespace fabric::async
