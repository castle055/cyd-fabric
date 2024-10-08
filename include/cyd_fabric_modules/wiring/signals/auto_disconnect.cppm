// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  auto_disconnect.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:auto_disconnect;

import std;

import fabric.ts.packs;

import :types;
import :connection;

class fabric::wiring::auto_disconnect {
public:
  auto_disconnect() = default;

  ~auto_disconnect() {
    for (auto&& connection: connections_) {
      connection.disconnect();
    }
  }

  template <typename...> friend class fabric::wiring::signal;
private:
  void add_connection(connection c) {
    connections_.emplace_front(c);
  }

  std::forward_list<connection> connections_{};
};
