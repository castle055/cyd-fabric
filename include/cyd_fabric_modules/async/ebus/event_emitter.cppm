// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  event_emitter.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:event_emitter;

import std;

import :types;
import :impl;

export namespace fabric::async {
  class ebus_producer {
  public:
    explicit ebus_producer(const ebus::wptr &ebus)
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

  private:
    ebus::wptr ebus_;
  };

  ebus_producer ebus::sptr::make_producer() const {
    return ebus_producer {*this};
  }
}