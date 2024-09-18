// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  _concepts.cppm
 *! \brief 
 *!
 */

export module fabric.wiring.signals:concepts;

import std;

import fabric.ts.packs;

import :types;

export namespace fabric::wiring {
  template <typename Signal>
  concept SignalConcept = ts::packs::is_type<signal, Signal>::value;

  template <template <SignalConcept...> typename Strategy>
  concept StrategyConcept = requires {
    typename Strategy<>::output_signal_type;
    typename Strategy<>::state_type;
    {
      Strategy<>::template on_signal<signal<>, 0>(
        std::declval<typename Strategy<>::state_type&>(), std::declval<std::tuple<>&>()
      )
    } -> std::same_as<void>;
    {
      Strategy<>::maybe_emit(
        std::declval<typename Strategy<>::state_type&>(),
        std::declval<typename Strategy<>::output_signal_type&>()
      )
    } -> std::same_as<void>;
  };
}