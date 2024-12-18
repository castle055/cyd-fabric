// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FABRIC_EVENTS_H
#define FABRIC_EVENTS_H


#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include "../testing/test_enabled.h"

using str = std::string;

namespace cyd::fabric::async {
    template<class T>
    concept EventType = requires {
      typename T::DataType;
      {T::type} -> std::convertible_to<str>;
      {T::data} -> std::convertible_to<typename T::DataType>;
    };
    
    template<typename T> requires EventType<T>
    struct ParsedEvent {
      using DataType = T::DataType;
      
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
      std::unique_ptr<uint8_t> ID;
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
        ID = std::unique_ptr<uint8_t>(new uint8_t);
      }
      //
      //listener_t(const listener_t &other) {
      //  ID = other.get_id();
      //}
      
      [[nodiscard]] long get_id() const {
        return (long)(uint8_t *)ID.get();
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


// MACROS
#define consumer [=](it)
#define listen(EVENT, ...) \
  on_event<EVENT>(cyd::fabric::async::Consumer<EVENT>( \
      [&](const cyd::fabric::async::ParsedEvent<EVENT>& ev) __VA_ARGS__))


struct event_data_type_base_t {
  unsigned long win = 0;
};
// TODO - In case of event name collision (it's possible), maybe append __FILE__ or __LINE__ to the `type`
// variable to make the event type unique even if multiple translation units declare similarly named events.
#define EVENT(NAME, DATA)                                                      \
  struct NAME {                                                                \
    constexpr static const char* type = #NAME;                                 \
    struct DataType: public event_data_type_base_t DATA data;                  \
  };


#endif//FABRIC_EVENTS_H
