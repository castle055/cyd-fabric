/*!
 ! Copyright (c) 2024, Víctor Castillo Agüero.
 ! This file is part of the Cydonia project.
 !
 ! This library is free software: you can redistribute it and/or modify
 ! it under the terms of the GNU General Public License as published by
 ! the Free Software Foundation, either version 3 of the License, or
 ! (at your option) any later version.
 !
 ! This library is distributed in the hope that it will be useful,
 ! but WITHOUT ANY WARRANTY; without even the implied warranty of
 ! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ! GNU General Public License for more details.
 !
 ! You should have received a copy of the GNU General Public License
 ! along with this library.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CYD_UI_ASYNC_H
#define CYD_UI_ASYNC_H

#include "common.h"

#include "cyd_fabric/async/async_bus.h"
using namespace cyd::fabric::async;

namespace test::async {
    inline std::unique_ptr<cyd::fabric::async::async_bus_t> make_async_bus() {
      return std::make_unique<cyd::fabric::async::async_bus_t>();
    }
    inline std::unique_ptr<cyd::fabric::async::event_queue_t> make_event_queue() {
      return std::make_unique<cyd::fabric::async::event_queue_t>();
    }
    inline std::unique_ptr<cyd::fabric::async::coroutine_runtime_t> make_coroutine_runtime() {
      return std::make_unique<cyd::fabric::async::coroutine_runtime_t>();
    }
}


#endif //CYD_UI_ASYNC_H
