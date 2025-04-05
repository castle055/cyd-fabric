// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  uring.cppm
 *! \brief
 *!
 */
module;
#include <liburing.h>
#include <stdexcept>

export module fabric.io.rings.uring.response;

import std;
import reflect;

export namespace platform {
  struct ioring_res {
    std::size_t user_data;
    std::size_t result;

    bool ok() const {
      return 0 == result;
    }

    void* user_data_as_ptr() const {
      return reinterpret_cast<void*>(user_data);
    }
  };
} // namespace platform
