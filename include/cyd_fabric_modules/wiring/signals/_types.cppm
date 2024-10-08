// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  _types.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:types;

import std;

export namespace fabric::wiring {
  template <typename R, typename... Args>
  class slot;

  template <typename... Args>
  class signal;

  class connection;

  class auto_disconnect;

  template <typename Owner, typename... Args>
  class private_signal;
  
  template <typename Owner, typename... Args>
  class output_signal;
  template <typename Owner, typename... Args>
  class input_signal;
} // namespace fabric::wiring

namespace fabric::wiring {
  class signal_base;

  using slot_id_t = std::uint32_t;
}