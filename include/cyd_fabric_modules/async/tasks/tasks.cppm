// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  tasks.cppm
 *! \brief
 *!
 */
export module fabric.tasks;

export import :types;
export import :task;
export import :schedule;
export import :continuation;
export import :executor;
export import :task_promise;
export import :async_lifetime;
export import :detached_task;

export import :observable;
export import :launch;
export import :when_all;
export import :when_any;
export import :with_cancellation;

export import :this_task.get_executor;
export import :this_task.switch_executor;
export import :this_task.resources;
export import :this_task.sleep;
export import :this_task.keep_alive;
export import :this_task.cancellation;

export namespace fabric::tasks {}

export namespace fabric {
  inline constexpr fabric::task<> (*noop_task)(){[] -> task<> { co_return; }};
} // namespace fabric
