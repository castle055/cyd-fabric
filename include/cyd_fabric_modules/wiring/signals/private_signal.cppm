// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  private_signal.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:private_signal;

import :types;
import :signal;

export template <typename Owner, typename... Args>
class fabric::wiring::private_signal: public signal<Args...> {
public:
  private_signal()  = default;
  ~private_signal() = default;

  using signal<Args...>::connect;

  friend Owner;

private:
  using signal<Args...>::emit;
};
