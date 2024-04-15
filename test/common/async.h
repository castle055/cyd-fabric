// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_UI_ASYNC_H
#define CYD_UI_ASYNC_H

#include "common.h"

#include "async/async_bus.h"
using namespace cydui::async;

namespace test::async {
    inline std::unique_ptr<cydui::async::async_bus_t> make_async_bus() {
      return std::make_unique<cydui::async::async_bus_t>();
    }
    inline std::unique_ptr<cydui::async::event_queue_t> make_event_queue() {
      return std::make_unique<cydui::async::event_queue_t>();
    }
    inline std::unique_ptr<cydui::async::coroutine_runtime_t> make_coroutine_runtime() {
      return std::make_unique<cydui::async::coroutine_runtime_t>();
    }
}


#endif //CYD_UI_ASYNC_H
