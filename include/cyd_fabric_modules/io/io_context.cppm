// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  io_context.cppm
 *! \brief
 *!
 */

export module fabric.io.io_context;

import std;
import reflect;

import fabric.logging;

export import fabric.io.io_worker;


export namespace fabric::io {
  enum io_context_type {
    WORKER_THREAD,
  };

  class io_context {
    std::variant<std::monostate, io_worker::sptr> io_handler_;
    io_context_type                               type_;

    explicit io_context(const io_worker::sptr& worker)
        : io_handler_(worker),
          type_(WORKER_THREAD) {}

  public:
    using sptr = std::shared_ptr<io_context>;

    template <io_context_type Type>
    static sptr make() {
      if constexpr (WORKER_THREAD == Type) {
        return std::shared_ptr<io_context>(new io_context(io_worker::make()));
      } else {
        static_assert(false, "invalid io context type");
      }
      return nullptr;
    }

    template <typename RequestType>
    task<int> make_request(const RequestType& request) {
      int res = 0xFF'FF'FF'FF;
      if (WORKER_THREAD == type_) {
        res = co_await std::get<io_worker::sptr>(io_handler_)->make_request(request);
      } else {
        throw std::runtime_error("invalid io context type");
      }
      co_return res;
    }
  };
} // namespace fabric::io

export namespace fabric::this_task {
  task<io::io_context::sptr> get_io_context() {
    co_return co_await get_resource<io::io_context>();
  }

  auto set_io_context(
    const io::io_context::sptr& ptr
  ) {
    //! Cannot be a task<> because then we would be overriding the resource for
    //! this task<> and not the parent, which is the one who wants the override.
    return set_resource<io::io_context>(ptr);
  }

  task<bool> has_io_context() {
    co_return co_await has_resource<io::io_context>();
  }
}
