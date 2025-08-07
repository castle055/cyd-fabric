// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  main_executor.cppm
 *! \brief
 *!
 */

export module fabric.main;

import std;
import reflect;

export import fabric.io;
export import fabric.logging;
export import fabric.tasks;
export import fabric.thread_name;

namespace fabric {
  std::atomic_flag      main_executor_initialized{false};
  tasks::executor::sptr exec{nullptr};
} // namespace fabric


void print_banner() {
  LOG::print{DEBUG}("########  ######  ########   #######   ####   ######  ");
  LOG::print{DEBUG}("##       ##   ##  ##     ##  ##    ##   ##   ##    ## ");
  LOG::print{DEBUG}("######  ########  ########   #######    ##   ##       ");
  LOG::print{DEBUG}("##      ##    ##  ##     ##  ##   ##    ##   ##    ## ");
  LOG::print{DEBUG}("##      ##    ##  ########   ##    ##  ####   ######  ");
  LOG::print{DEBUG}("");
}


export namespace fabric {
  int main(auto&& main_task) {
    print_banner();
    if (main_executor_initialized.test_and_set()) {
      throw std::logic_error("main_executor already initialized");
    }

    auto main_exec_thread = std::make_shared<tasks::main_executor_thread_t>();
    exec                  = tasks::executor::make(main_exec_thread);
    LOG::print{DEBUG}("Initialized main executor");

    const io::io_context::sptr io_context = io::io_context::make<io::WORKER_THREAD>();
    LOG::print{DEBUG}("Initialized IO context (WORKER_THREAD)");

    exec->get_spawn_context()->set_resource(io_context);
    LOG::print{DEBUG}("Attached IO context to main executor");

    auto t = exec->schedule([task = std::move(main_task)] -> fabric::task<int> {
      LOG::print{DEBUG}("Main task started");
      auto ka_token = co_await this_task::keep_alive();
      auto res = co_await task();
      LOG::print{DEBUG}("Main task done (returned {})", res);
      co_return res;
    });

    LOG::print{DEBUG}("Entering main executor");
    main_exec_thread->run();
    LOG::print{DEBUG}("Main executor stopped");

    auto result = t.get();
    exec->join();

    if (result == 0) {
      LOG::print{INFO}("Main task completed successfully");
    } else {
      LOG::print{INFO}("Main task finished with error code: {}", result);
    }
    return result;
  }

  tasks::executor::sptr get_main_executor() {
    return exec;
  }
} // namespace fabric
