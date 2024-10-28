// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  raw_listener.cppm
 *! \brief 
 *!
 */

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async.ebus:raw_listener;

import std;

import :types;
import :event;

export namespace fabric::async {
  class raw_listener {
  private TEST_PUBLIC:
    std::unique_ptr<std::uint8_t> ID;
    bool active = true;

  public:
    using sptr = std::shared_ptr<raw_listener>;
    using wptr = std::weak_ptr<raw_listener>;

    raw_listener(): ebus_(nullptr) {
      ID     = 0;
      active = false;
    }

    explicit raw_listener(ebus* event_queue_, std::string type, raw_event_handler c)
      : ebus_(event_queue_),
        event_type_(std::move(type)),
        func_(new raw_event_handler {std::move(c)}) {
      ID = std::unique_ptr<std::uint8_t>(new std::uint8_t);
    }

    ~raw_listener() {
      remove();
    }
    //
    //listener_t(const listener_t &other) {
    //  ID = other.get_id();
    //}

    [[nodiscard]] long get_id() const {
      return (long)(std::uint8_t*)ID.get();
    }

    void remove();

    bool is_active() const {
      return active;
    }

    void operator()(const event::sptr& ev) const {
      if (nullptr != func_ && active) {
        func_->operator()(*ev.get());
      }
    }

    const std::string& event_type() const {
      return event_type_;
    }
  private:
    ebus* const ebus_;
    std::string event_type_;
    raw_event_handler* func_ = nullptr;
  };

}