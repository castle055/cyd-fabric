// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  _ts.cppm
 *! \brief 
 *!
 */

export module fabric.wiring.signals:ts;

import :concepts;

export namespace fabric::wiring {
  template <SignalConcept Signal>
  using signal_args = typename Signal::args_tuple_type;
}