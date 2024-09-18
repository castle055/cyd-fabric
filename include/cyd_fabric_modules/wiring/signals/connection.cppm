// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  connection.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:connection;

import std;

import :types;
import :concepts;


class fabric::wiring::connection {
  template <SignalConcept Signal>
  static bool disconnect_hook(void* signal, slot_id_t slot_id) {
    return static_cast<Signal*>(signal)->disconnect(slot_id);
  }
public:
  template <typename... Args>
  friend class signal;

  bool disconnect() {
    if (not connected_)
      return false;

    connected_ = false;
    return disconnect_function_(signal_, slot_id_);
  }

private:
  // template <SignalConcept S>
  // connection(S* signal, slot_id_t slot_id)
  //     : disconnect_function_([signal, slot_id] -> bool { return signal->disconnect(slot_id); })
  //     {}

  template <SignalConcept S>
  connection(S* signal, slot_id_t slot_id)
      : disconnect_function_(&disconnect_hook<S>),
        signal_(signal),
        slot_id_(slot_id) {}


  bool      (*disconnect_function_)(void*, slot_id_t);
  void*     signal_;
  slot_id_t slot_id_;
  bool      connected_ = true;

  char padding_[3];
};

