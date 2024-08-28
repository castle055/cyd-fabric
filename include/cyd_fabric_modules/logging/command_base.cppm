// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  command_base.cppm
 *! \brief 
 *!
 */

export module fabric.logging:command_base;
export import :context;

export namespace LOG {
  void log_entry(const entry_t &entry) {
    for (const auto &filter: config::filters) {
      if (std::regex_match(entry.path.string(), filter.pattern_)) {
        config::targets.at(filter.target_.id_hash)->append(entry);
      }
    }
  }
}
