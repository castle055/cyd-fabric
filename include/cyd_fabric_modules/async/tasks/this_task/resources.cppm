// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  resources.cppm
 *! \brief 
 *!
 */

export module fabric.tasks:this_task.resources;
import :task;
import :task_promise;
import :executor;

import std;
import reflect;

template <typename ResourceType>
struct awaitable_get_resource {
  std::shared_ptr<ResourceType>                 val;
  fabric::tasks::task_resource_id<ResourceType> id;

  explicit awaitable_get_resource(fabric::tasks::task_resource_id<ResourceType> id_ = {})
      : id(id_) {}

  bool await_ready() const noexcept {
    return false;
  }
  template <typename P>
  bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    val = h.promise().get_resource(id);
    return false;
  }
  std::shared_ptr<ResourceType> await_resume() const noexcept {
    return val;
  }
};

template <typename ResourceType>
struct awaitable_set_resource {
  std::shared_ptr<ResourceType>                 val;
  fabric::tasks::task_resource_id<ResourceType> id;

  explicit awaitable_set_resource(
    const std::shared_ptr<ResourceType>&          ptr_,
    fabric::tasks::task_resource_id<ResourceType> id_ = {}
  )
      : val(ptr_),
        id(id_) {}

  bool await_ready() const noexcept {
    return false;
  }
  template <typename P>
  bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    h.promise().set_resource(val, id);
    return false;
  }
  void await_resume() const noexcept {}
};

template <typename ResourceType>
struct awaitable_has_resource {
  bool                                          val;
  fabric::tasks::task_resource_id<ResourceType> id;

  explicit awaitable_has_resource(fabric::tasks::task_resource_id<ResourceType> id_ = {})
      : id(id_) {}

  bool await_ready() const noexcept {
    return false;
  }
  template <typename P>
  bool await_suspend(fabric::tasks::task_handle<P> h) noexcept {
    val = h.promise().has_resource(id);
    return false;
  }
  bool await_resume() const noexcept {
    return val;
  }
};


export namespace fabric::this_task {
  template <typename ResourceType>
  task<std::shared_ptr<ResourceType>> get_resource(tasks::task_resource_id<ResourceType> id = {}) {
    co_return co_await awaitable_get_resource<ResourceType>{id};
  }

  template <typename ResourceType>
  awaitable_set_resource<ResourceType> set_resource(
    const std::shared_ptr<ResourceType>& ptr, tasks::task_resource_id<ResourceType> id = {}
  ) {
    //! Cannot be a task<> because then we would be overriding the resource for
    //! this task<> and not the parent, which is the one who wants the override.
    return awaitable_set_resource<ResourceType>{ptr, id};
  }

  template <typename ResourceType>
  task<bool> has_resource(tasks::task_resource_id<ResourceType> id = {}) {
    co_return co_await awaitable_has_resource<ResourceType>{id};
  }
}
