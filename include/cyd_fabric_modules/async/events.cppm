// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async:events;

import std;

import fabric.type_aliases;

using str = std::string;

export namespace cyd::fabric::async {
    template<class T>
    concept EventType = requires {
      typename T::DataType;
      {T::type} -> std::convertible_to<str>;
      {T::data} -> std::convertible_to<typename T::DataType>;
    };
    
    template<typename T> requires EventType<T>
    struct ParsedEvent {
      using DataType = typename T::DataType;
      
      const str &type;
      const typename T::DataType* data;
    };
    
    enum EventStatus {
      PENDING,
      PROCESSING,
      CONSUMED,
    };
    
    class event_impl_t {
    public:
      // Unfortunately needed since `ev` can be of any type
      std::function<void()> ev_destructor = []() { };
      ~event_impl_t() {
        ev_destructor();
      }
      
      str type;
      std::atomic<EventStatus> status = PENDING;
      void* ev = nullptr;
      
      template<EventType T>
      ParsedEvent<T> parse() {
        return {
          .type = type,
          .data = (T::type == type) ? (typename T::DataType*) ev : nullptr,
        };
      }
    };
    
    using event_t = std::shared_ptr<event_impl_t>;
    class event_queue_t;
    
    inline event_t make_event(const str &event_type, void* data, std::function<void()> &&data_destructor) {
      auto ev = std::make_shared<event_impl_t>();
      ev->type = event_type;
      ev->ev = data;
      ev->ev_destructor = std::move(data_destructor);
      return ev;
    }
    
    template<typename T> requires EventType<T>
    using Consumer = std::function<void(const ParsedEvent<T> &)>;
    
    typedef std::function<void(event_t &)> Listener;
    
    class listener_t {
    private TEST_PUBLIC:
      std::unique_ptr<u8> ID;
      bool active = true;
    public:
      listener_t(): event_queue(nullptr) {
        ID = 0;
        active = false;
      }
      
      explicit listener_t(event_queue_t* event_queue_, str type, Listener c)
        : event_queue(event_queue_),
          event_type(std::move(type)),
          func(new Listener {std::move(c)}) {
        ID = std::unique_ptr<u8>(new u8);
      }
      //
      //listener_t(const listener_t &other) {
      //  ID = other.get_id();
      //}
      
      [[nodiscard]] long get_id() const {
        return (long)(u8 *)ID.get();
      }
      
      event_queue_t* const event_queue;
      str event_type;
      Listener* func = nullptr;
      
      void remove();
      
      bool is_active() const {
        return active;
      }
      
      void operator()(event_t ev) const {
        if (nullptr != func && active) {
          func->operator()(ev);
        }
      }
    };
  
}// namespace cydui::async
