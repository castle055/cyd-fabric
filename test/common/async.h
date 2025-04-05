// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_ASYNC_H
#define CYD_UI_ASYNC_H

#include "common.h"

import fabric.async;

namespace test::async {
    inline std::unique_ptr<fabric::async::async_bus_t> make_async_bus() {
      return std::unique_ptr<fabric::async::async_bus_t>(new fabric::async::async_bus_t{});
    }
    inline std::shared_ptr<fabric::async::ebus> make_event_queue() {
      return fabric::async::make_ebus();
    }
}


#endif //CYD_UI_ASYNC_H
