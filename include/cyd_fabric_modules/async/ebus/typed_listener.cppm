// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  typed_listener.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:typed_listener;

import :types;
export import :raw_listener;

export namespace fabric::async {
  template<EventType EV>
  class typed_listener {
  public:
    explicit typed_listener(const raw_listener::sptr& raw_listener)
      : raw_listener_(raw_listener) { }

    void remove() { // Should not be 'const'
      raw_listener_->remove();
    }

    raw_listener::sptr raw() const {
      return raw_listener_;
    }
  private:
    raw_listener::sptr raw_listener_;
  };
}