// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  event_listener.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:event_listener;

import std;

import :types;
import :impl;
export import :typed_listener;

export namespace fabric::async {
  class ebus_consumer {
  public:
    explicit ebus_consumer(const ebus::wptr &ebus)
      : ebus_(ebus) {
    }

    inline auto on_event(
      auto &&c
    ) const -> listener<event_type_from_handler<decltype(c)>> requires (
      EventType<event_type_from_handler<decltype(c)>>
    ) {
      return ebus_.lock()->on_event(c);
    }

    template<typename T>
    inline auto on_event(
      auto &&c
    ) const -> listener<event_type_from_handler<decltype(c)>> requires (
      EventType<event_type_from_handler<decltype(c)>> &&
      std::same_as<T, event_type_from_handler<decltype(c)>
      >
    ) {
      return ebus_.lock()->on_event(c);
    }

    void remove_listener(const raw_listener &listener) const {
      ebus_.lock()->remove_listener(listener);
    }

  private:
    ebus::wptr ebus_;
  };

  ebus_consumer ebus::sptr::make_consumer() const {
    return ebus_consumer {*this};
  }
}