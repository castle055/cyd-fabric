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

  io_task<int> socket(int domain, int type, int protocol) {
    co_return co_await make_request(
      request_types::socket{.domain = domain, .type = type, .protocol = protocol}
    );
  }

  io_task<int> bind(file_decriptor sockfd, socket_address& addr) {
    co_return co_await make_request(
      request_types::bind{.sockfd = sockfd, .addr = addr.address, .addrlen = addr.length}
    );
  }

  io_task<int> listen(file_decriptor sockfd, int backlog) {
    co_return co_await make_request(request_types::listen{.sockfd = sockfd, .backlog = backlog});
  }

  io_task<int> accept(file_decriptor sockfd, socket_address& addr, int flags) {
    co_return co_await make_request(request_types::accept{
      .sockfd = sockfd, .addr = addr.address, .addrlen = &addr.length, .flags = flags
    });
  }

  io_task<int> connect(file_decriptor sockfd, const socket_address& addr) {
    co_return co_await make_request(request_types::connect{
      .sockfd = sockfd, .addr = addr.address, .addrlen = addr.length
    });
  }

  io_task<int> send(file_decriptor sockfd, const void* buffer, std::size_t length, int flags) {
    co_return co_await make_request(request_types::send{
      .sockfd = sockfd,
      .buf = buffer,
      .len = length,
      .flags = flags
    });
  }

  io_task<int> send_zc(file_decriptor sockfd, const void* buffer, std::size_t length, int flags, unsigned int zc_flags) {
    co_return co_await make_request(request_types::send_zc{
      .sockfd = sockfd,
      .buf = buffer,
      .len = length,
      .flags = flags,
      .zc_flags = zc_flags
    });
  }

  io_task<int> sendto(file_decriptor sockfd, const void* buffer, std::size_t length, int flags, const socket_address& addr) {
    co_return co_await make_request(request_types::sendto{
      .sockfd = sockfd,
      .buf = buffer,
      .len = length,
      .flags = flags,
      .addr = addr.address,
      .addrlen = static_cast<std::uint16_t>(addr.length)
    });
  }

  io_task<int> send_bundle(file_decriptor sockfd, std::size_t length, int flags) {
    co_return co_await make_request(request_types::send_bundle{
      .sockfd = sockfd,
      .len = length,
      .flags = flags
    });
  }

  io_task<int> send_set_addr(const socket_address& addr) {
    co_return co_await make_request(request_types::send_set_addr{
      .dest_addr = addr.address,
      .addr_len = static_cast<std::uint16_t>(addr.length)
    });
  }

  io_task<int> recv(file_decriptor sockfd, void* buffer, std::size_t length, int flags) {
    co_return co_await make_request(request_types::recv{
      .sockfd = sockfd,
      .buf = buffer,
      .len = length,
      .flags = flags
    });
  }

} // namespace fabric::io
