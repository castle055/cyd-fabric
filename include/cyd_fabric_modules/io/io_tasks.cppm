// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  io_tasks.cppm
 *! \brief
 *!
 */

export module fabric.io.tasks;

import std;
import reflect;
import reflect.serialize;

import fabric.logging;
export import fabric.io.io_context;
export import fabric.io.error;

export namespace fabric::io {
  template <typename T = void>
  using io_result = result<io::error, T>;

  template <typename R = void>
  using io_task = task<io_result<R>>;

  template <typename RequestType>
  io_task<int> make_request(const RequestType& req) {
    LOG::print{DEBUG
    }(">> IO: {} {}",
      refl::type_name<RequestType>,
      refl::serializer<formats::json_fmt>::to_string(req));
    auto ka_token = co_await this_task::keep_alive(); // keep executor alive until request is done
    auto io_ctx   = co_await this_task::get_io_context();
    auto result   = co_await io_ctx->make_request(req);
    if (result < 0) {
      auto err = error::from_code(-result);
      LOG::print{WARN}("<< IO: {}", err.to_string());
      co_return err;
    }
    LOG::print{DEBUG}("<< IO: {}", result);
    co_return result;
  }

  io_task<file_decriptor> open(std::filesystem::path path) {
    co_return co_await make_request(request_types::open{path});
  }

  using namespace std::chrono_literals;
  io_task<> close(file_decriptor fd) {
    co_return (co_await make_request(request_types::close{fd})).map_value<void>();
  }

  io_task<int> read(file_decriptor fd, void* buffer, unsigned int size, std::size_t offset) {
    co_return co_await make_request(
      request_types::read{.fd = fd, .buf = buffer, .nbytes = size, .offset = offset}
    );
  }

  io_task<int> write(file_decriptor fd, const void* buffer, unsigned int size, std::size_t offset) {
    co_return co_await make_request(
      request_types::write{.fd = fd, .buf = buffer, .nbytes = size, .offset = offset}
    );
  }

} // namespace fabric::io
