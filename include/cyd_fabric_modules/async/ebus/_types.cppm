// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  _types.cppm
 *! \brief
 *!
 */

export module fabric.async.ebus:types;

import std;

export import fabric.templates.functor_arguments;
export import fabric.ts.apply;

export import :concepts;

export namespace fabric::async {
  class event;

  using raw_event_handler = std::function<void(const event&)>;


  class ebus;
  class ebus_actor;
  class ebus_runner;
  class ebus_producer;
  class ebus_consumer;


  class raw_listener;

  template <EventType EV>
  class typed_listener;

  template <EventType EV>
  using listener = typed_listener<EV>;


  enum class EventStatus {
    PENDING,
    PROCESSING,
    CONSUMED,
  };

  // clang-format off
  template <typename F>
  using event_type_from_handler =
    typename ts::with_type<F>
      ::template apply<std::remove_reference>
      ::template apply<first_argument>
      ::template apply<std::remove_reference>
      ::template apply<std::remove_const>
      ::done;
  // clang-format on
}

