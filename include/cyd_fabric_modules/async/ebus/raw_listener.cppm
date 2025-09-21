// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  raw_listener.cppm
 *! \brief
 *!
 */

export module fabric.async.ebus:raw_listener;

import std;
import reflect;

import :types;
import :event;

export namespace fabric::async {
  class raw_listener {
  public:
    using sptr = std::shared_ptr<raw_listener>;
    using wptr = std::weak_ptr<raw_listener>;

    raw_listener()
        : ebus_(nullptr) {
      ID     = 0;
      active = false;
    }

    explicit raw_listener(ebus* event_queue_, const std::string& type, auto c)
        : ebus_(event_queue_),
          event_type_(type),
          func_(new raw_event_handler{c}) {
      ID = std::unique_ptr<std::uint8_t>(new std::uint8_t);
    }

    ~raw_listener() {
      remove();
    }
    //
    // listener_t(const listener_t &other) {
    //  ID = other.get_id();
    //}

    [[nodiscard]] long get_id() const {
      return (long)(std::uint8_t*)ID.get();
    }

    void remove();

    bool is_active() const {
      return active;
    }

    task<> operator()(const tasks::executor& exec, const event::sptr& ev) const {
      if (nullptr != func_ && active) {
        exec.schedule([=,this] -> task<> {
          if (nullptr != func_ && active) {
            co_await func_->operator()(*ev);
          }
          co_return;
        }()).detach();
      }
      co_return;
    }

    const std::string& event_type() const {
      return event_type_;
    }

  private:
    std::unique_ptr<std::uint8_t> ID;
    bool                          active = true;

    ebus* const        ebus_;
    std::string        event_type_;
    raw_event_handler* func_ = nullptr;
  };

} // namespace fabric::async
