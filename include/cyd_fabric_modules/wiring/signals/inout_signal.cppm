// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  private_signal.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:inout_signal;

import :types;
import :signal;

export template <typename Owner, typename... Args>
class fabric::wiring::output_signal: public signal<Args...> {
public:
  output_signal()  = default;
  ~output_signal() = default;

  using signal<Args...>::connect;

  friend Owner;

private:
  using signal<Args...>::emit;
};

export template <typename Owner, typename... Args>
class fabric::wiring::input_signal: public signal<Args...> {
public:
  input_signal()  = default;
  ~input_signal() = default;

  using signal<Args...>::emit;

  friend Owner;

private:
  using signal<Args...>::connect;
};
