// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ebus_impl.cppm
 *! \brief 
 *!
 */

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async.ebus:impl;

import std;

import :types;
export import :event;
export import :raw_listener;
export import :typed_listener;

export namespace fabric::async {
  class ebus {
  public:
    using wptr = std::weak_ptr<ebus>;
    struct sptr: std::shared_ptr<ebus> {
      explicit sptr(ebus* ev_queue): shared_ptr<fabric::async::ebus>(ev_queue) {
      }

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
  protected TEST_PUBLIC:
    ebus() = default;

  private TEST_PUBLIC: /// @name Raw Event Handling
    // ? This function creates a copy of the eve, thus increasing its ref count.
    void push_event(const fabric::async::event::sptr &ev) { {
        std::scoped_lock lk {event_mutex};
        front_ebus.push(ev);
      }
      // log_task.debug("NEW EVENT: %s", ev->type.c_str());
    }

    event::sptr &emit_raw(event::sptr &ev) {
      push_event(ev);
      return ev;
    }

    event::sptr emit_raw(const std::string &event_type, void* data, std::function<void()> &&data_destructor) {
      event::sptr ev = make_event(event_type, data, std::forward<std::function<void()> &&>(data_destructor));
      push_event(ev);
      return ev;
    }

    raw_listener::sptr on_event_raw(const std::string &event_type, const raw_event_handler &l_) {
      std::scoped_lock lk {listeners_mutex};
      if (!event_listeners.contains(event_type))
        event_listeners.insert({event_type, { }});
      auto l = std::make_shared<raw_listener>(this, event_type, l_);
      event_listeners[event_type].emplace_back(l);
      return l;
    }

    raw_listener::sptr on_event_raw(const std::string &event_type, raw_event_handler &&l_) {
      return on_event_raw(event_type, std::forward<const raw_event_handler&>(l_));
    }

  private TEST_PUBLIC: /// @name Event Processing
    void swap_ebuss() {
      std::scoped_lock lk {event_mutex};
      std::swap(front_ebus, back_ebus);
    }

    std::vector<std::shared_ptr<raw_listener>> get_listeners_for_event(const std::string &ev_type) {
      std::vector<std::shared_ptr<raw_listener>> listeners { };

      if (event_listeners.contains(ev_type)) {
        const auto &ev_listeners = event_listeners[ev_type];
        listeners.reserve(ev_listeners.size());
        for (const auto &item: ev_listeners) {
          listeners.emplace_back(item);
        }
      }

      return listeners;
    }

    void process_event(const event::sptr&ev) {
      ev->status = EventStatus::PROCESSING;
      //{ // Make copy of listeners with the mutex
      //  std::scoped_lock lk {listeners_mutex};
      listeners_mutex.lock();
      std::vector<std::shared_ptr<raw_listener>> listeners = get_listeners_for_event(ev->type);
      listeners_mutex.unlock();
      //}

      // Iterate over copy of listeners list. This should allow any listener to modify the listeners list (ie: removing themselves)
      for (const auto &listener: listeners) {
        listener->operator()(ev);
      }

      // Clean up event
      ev->status = EventStatus::CONSUMED;
    }

    void process_all_events(std::queue<event::sptr> &ev_queue) {
      while (!ev_queue.empty()) {
        process_event(ev_queue.front());
        ev_queue.pop();
      }
    }

  protected TEST_PUBLIC: /// @name Bus Interface
    void events_process_batch() {
      swap_ebuss();
      process_all_events(back_ebus);
    }

  public: /// @name Public Interface
    template<EventType T>
    inline event::sptr emit() {
      auto* data_ptr = new T();
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    template<EventType T>
    inline event::sptr emit(const T &event) {
      auto* data_ptr = new T(event);
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    template<EventType T>
    inline event::sptr emit(T &&event) {
      auto* data_ptr = new T(std::forward<T &&>(event));
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    template<EventType T, typename... EVFields>
    inline event::sptr emit(EVFields &&... fields) {
      auto* data_ptr = new T(std::forward<EVFields &&>(fields)...);
      return emit_raw(T::type, data_ptr, [data_ptr]() { delete data_ptr; });
    }

    inline auto on_event(
      auto &&c
    ) -> listener<event_type_from_handler<decltype(c)>> requires (
      EventType<event_type_from_handler<decltype(c)>>
    ) {
      using EventT = event_type_from_handler<decltype(c)>;
      return listener<EventT> {
        on_event_raw(EventT::type, [&, c](const event &ev) {
          c(ev.as<EventT>());
        })
      };
    }

    template<typename T>
    inline auto on_event(
      auto &&c
    ) -> listener<event_type_from_handler<decltype(c)>> requires (
      EventType<event_type_from_handler<decltype(c)>> &&
      std::same_as<T, event_type_from_handler<decltype(c)>
      >
    ) {
      return on_event(c);
    }

    void remove_listener(const raw_listener &listener) {
      if (listener.get_id() == 0) return;
      const std::string &event_type = listener.event_type();
      std::scoped_lock lk {listeners_mutex};
      if (event_listeners.contains(event_type)) {
        for (auto l = event_listeners[event_type].begin(); l != event_listeners[event_type].end(); ++l) {
          if (l->lock()->get_id() == listener.get_id()) {
            event_listeners[event_type].erase(l);
            return;
          }
        }
      }
    }

    bool is_runner_claimed() const {
      return runner_claimed_ != nullptr;
    }

    void evict_runner() {
      runner_claimed_ = nullptr;
    }

  public: /// @name Getter
    ebus &get_ebus() {
      return *this;
    }

  public: /// @name Destructor
    ~ebus() {
      for (auto &item: event_listeners) {
        auto listeners = get_listeners_for_event(item.first);
        for (auto &l: listeners) {
          l->remove();
        }
      }
    }

  private TEST_PUBLIC:
    std::queue<event::sptr> front_ebus { };
    std::queue<event::sptr> back_ebus { };

    std::unordered_map<
      std::string,
      std::deque<raw_listener::wptr>
    > event_listeners { };

  private TEST_PUBLIC:
    std::mutex event_mutex;
    std::mutex listeners_mutex;

  private TEST_PUBLIC:
    ebus_runner* runner_claimed_ = nullptr;
  };

  void raw_listener::remove() {
    if (nullptr != ebus_) ebus_->remove_listener(*this);
    delete func_;
    func_  = nullptr;
    active = false;
  }

  ebus::sptr make_ebus() {
    return ebus::sptr(new ebus());
  }
}