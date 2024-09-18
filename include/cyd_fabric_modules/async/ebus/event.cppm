// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  event.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:event;

import std;

import :types;

export namespace fabric::async {
  class event {
  public:
    using sptr = std::shared_ptr<event>;

    // Unfortunately needed since `ev` can be of any type
    std::function<void()> ev_destructor = []() {
    };

    ~event() {
      ev_destructor();
    }

    std::string type                = "";
    std::atomic<EventStatus> status = EventStatus::PENDING;
    void* ev                        = nullptr;

    template<EventType T>
    const T& as() const {
      return *static_cast<T*>(ev);
    }
  };

  inline event::sptr make_event(const std::string &event_type, void* data, std::function<void()> &&data_destructor) {
    auto ev           = std::make_shared<event>();
    ev->type          = event_type;
    ev->ev            = data;
    ev->ev_destructor = std::move(data_destructor);
    return ev;
  }
}