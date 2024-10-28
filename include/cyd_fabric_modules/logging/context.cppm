// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  context.cppm
 *! \brief 
 *!
 */

export module fabric.logging:context;

export import :filter;

export import fabric.logging.entry;
export import fabric.logging.levels;
export import fabric.logging.target_base;

import fabric.logging.targets.stdout;
import fabric.logging.targets.stderr;

namespace LOG::config {
  std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> default_targets() {
    std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> ts { };
    ts.emplace(target_id {"stdout"}.id_hash, TARGETS::STDOUT::BUILDER { }());
    ts.emplace(target_id {"stderr"}.id_hash, TARGETS::STDERR::BUILDER { }());
    return ts;
  }

  std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> targets = default_targets();
  std::vector<filter_t> filters { };
}
