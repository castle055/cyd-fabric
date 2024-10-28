// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ebus_actor.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:actor;

import std;

import :types;
import :impl;
import :event_emitter;
import :event_listener;

export namespace fabric::async {
  class ebus_actor {
  public:
    explicit ebus_actor(const ebus::wptr &ebus)
      : ebus_(ebus) {
    }

    template<EventType T>
    inline event::sptr emit() const {
      return ebus_.lock()->emit<T>();
    }

    template<EventType T>
    inline event::sptr emit(const T &event) const {
      return ebus_.lock()->emit<T>(event);
    }

    template<EventType T>
    inline event::sptr emit(T &&event) const {
      return ebus_.lock()->emit<T>(event);
    }

    template<EventType T, typename... EVFields>
    inline event::sptr emit(EVFields &&... fields) const {
      return ebus_.lock()->emit<T>(std::forward<EVFields&&>(fields)...);
    }

    inline auto on_event(
      auto &&c
    ) -> listener<event_type_from_handler<decltype(c)>> requires (
      EventType<event_type_from_handler<decltype(c)>>
    ) {
      return ebus_.lock()->on_event(c);
    }

    template<typename T>
    inline auto on_event(
      auto &&c
    ) -> listener<event_type_from_handler<decltype(c)>> requires (
      EventType<event_type_from_handler<decltype(c)>> &&
      std::same_as<T, event_type_from_handler<decltype(c)>
      >
    ) {
      return ebus_.lock()->on_event(c);
    }

    void remove_listener(const raw_listener &listener) {
      ebus_.lock()->remove_listener(listener);
    }

    ebus_producer make_emitter() const {
      return ebus_producer{ebus_};
    }

    ebus_consumer make_listener() const {
      return ebus_consumer{ebus_};
    }
  private:
    ebus::wptr ebus_;
  };

  ebus_actor ebus::sptr::make_actor() const {
    return ebus_actor {*this};
  }
}