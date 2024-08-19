// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 8/19/24.
//

#ifndef ASYNC_EVENTS_H
#define ASYNC_EVENTS_H

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

#endif //ASYNC_EVENTS_H
