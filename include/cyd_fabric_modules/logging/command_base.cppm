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
      auto &constraint = filter.constraint_;
      if (constraint.matches_path(entry.path)
          && constraint.matches_function(entry.function)
          && constraint.matches_message(entry.message)
          && constraint.matches_linenum(entry.linenum)
          && constraint.matches_level(entry.level)
      ) {
        config::targets.at(filter.target_.id_hash)->append(entry);
      }
    }
  }
}

export
{
  constexpr const char* normalize(const char* path, const char* path_ref) {
    std::size_t i = 0;
    while (path[i] == path_ref[i] && path[i] != '\0' && path_ref[i] != '\0') {
      ++i;
    }
    return path + i;
  }
}
