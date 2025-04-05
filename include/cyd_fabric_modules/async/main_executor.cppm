// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  main_executor.cppm
 *! \brief
 *!
 */

export module fabric.main;

import std;
import reflect;

export import fabric.logging;
export import fabric.tasks;
export import fabric.io;

namespace fabric {
  std::atomic_flag      main_executor_initialized{false};
  tasks::executor::sptr exec;
} // namespace fabric

export namespace fabric {
  class main_executor {
    static void print_banner() {
      LOG::print{DEBUG}("########  ######  ########   #######   ####   ######  ");
      LOG::print{DEBUG}("##       ##   ##  ##     ##  ##    ##   ##   ##    ## ");
      LOG::print{DEBUG}("######  ########  ########   #######    ##   ##       ");
      LOG::print{DEBUG}("##      ##    ##  ##     ##  ##   ##    ##   ##    ## ");
      LOG::print{DEBUG}("##      ##    ##  ########   ##    ##  ####   ######  ");
      LOG::print{DEBUG}("");

      // LOG::print{DEBUG}("                                               v0.1.12 ");
    }

  public:
    static int entrypoint(auto&& main_task) {
      print_banner();
      if (main_executor_initialized.test_and_set()) {
        throw std::logic_error("main_executor already initialized");
      }

      exec = tasks::executor::make();
      LOG::print{DEBUG}("Initialized main executor");
      const io::io_context::sptr io_context = io::io_context::make<io::WORKER_THREAD>();
      LOG::print{DEBUG}("Initialized IO context (WORKER_THREAD)");
      exec->get_spawn_context()->set_resource(io_context);
      LOG::print{DEBUG}("Attached IO context to main executor.");

      auto t = exec->schedule([task = std::move(main_task)] -> fabric::task<int> {
        LOG::print{DEBUG}("Main task started.");
        auto res = co_await task();
        LOG::print{DEBUG}("Main task done ({}).", res);
        co_return res;
      });

      auto result = t.get();
      exec->join();

      if (result == 0) {
        LOG::print{DEBUG}("Main task completed successfully.");
      } else {
        LOG::print{ERROR}("Main task finished with error code: {}", result);
      }
      return result;
    }
  };
} // namespace fabric
