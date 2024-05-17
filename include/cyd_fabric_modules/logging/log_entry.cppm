// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  log_entry.cppm
 *! \brief 
 *!
 */
module;
#include <string>
#include <chrono>
#include <filesystem>

export module fabric.logging.entry;

export namespace LOG {
  struct entry_t {
    std::chrono::system_clock::time_point timestamp;
    std::filesystem::path path;
    unsigned int linenum;
    std::string function;
    std::string message;
  };
}
