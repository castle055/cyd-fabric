// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  filter.cppm
 *! \brief 
 *!
 */

export module fabric.logging:filter;
import fabric.logging.target_base;

export namespace LOG {
  struct filter_t {
    std::regex pattern_;
    target_id target_ {"default"};

    filter_t(const char*&&pattern, const char*&&target)
      : pattern_(pattern), target_(target) {
    }
  };
}
