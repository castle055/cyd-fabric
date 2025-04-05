// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  uring.cppm
 *! \brief
 *!
 */
module;
#include <liburing.h>
#include <stdexcept>

export module fabric.io.rings.uring;

import std;
import reflect;

export import fabric.tasks;
export import fabric.io.rings.requests;

import fabric.io.rings.uring.req_detail;
import fabric.io.rings.uring.response;

namespace platform {
  constexpr std::size_t DEFAULT_IORING_SIZE = 512;

  class ioring {
    struct io_uring ring_{};
    bool            moved = false;

  public:
    explicit ioring(std::size_t ring_size = DEFAULT_IORING_SIZE) {
      if (0 != io_uring_queue_init(ring_size, &ring_, 0)) {
        throw std::runtime_error("failed to initialize io_uring");
      }
    }
    ~ioring() {
      if (not moved) {
        io_uring_queue_exit(&ring_);
      }
    }
    ioring(const ioring&)            = delete;
    ioring& operator=(const ioring&) = delete;
    ioring(ioring&& other) noexcept {
      this->ring_ = other.ring_;
      this->moved = other.moved;
      other.ring_ = {};
      other.moved = true;
    }
    ioring& operator=(ioring&& other) noexcept {
      this->ring_ = other.ring_;
      this->moved = other.moved;
      other.ring_ = {};
      other.moved = true;
      return *this;
    }

    template <typename RequestType>
    void submit(const RequestType& request, __u64 user_data) {
      struct io_uring_sqe* sqe = io_uring_get_sqe(&ring_);
      ioring_req_detail<RequestType>::prepare(sqe, request);
      io_uring_sqe_set_data64(sqe, user_data);
      io_uring_submit(&ring_);
    }

    template <typename RequestType>
    void submit(const RequestType& request, void* user_data) {
      submit(request, reinterpret_cast<unsigned long>(user_data));
    }

    struct submission_t {
      struct io_uring*     ring;
      struct io_uring_sqe* sqe;

      template <typename RequestType>
      void prepare(const RequestType& request, __u64 user_data) {
        ioring_req_detail<RequestType>::prepare(sqe, request);
        io_uring_sqe_set_data64(sqe, user_data);
      }

      template <typename RequestType>
      void prepare(const RequestType& request, void* user_data) {
        prepare(request, reinterpret_cast<unsigned long>(user_data));
      }
    };
    submission_t begin_submit() {
      return {&ring_, io_uring_get_sqe(&ring_)};
    }
    void submit_all() {
      io_uring_submit(&ring_);
    }

    std::vector<ioring_res> poll() {
      const auto                        count = io_uring_cq_ready(&ring_);
      std::vector<struct io_uring_cqe*> entries{count};
      std::vector<ioring_res>           responses{};
      responses.reserve(count);

      io_uring_peek_batch_cqe(&ring_, entries.data(), count);

      for (const auto& entry: entries) {
        responses.emplace_back(io_uring_cqe_get_data64(entry), entry->res);
        io_uring_cqe_seen(&ring_, entry);
      }

      return std::move(responses);
    }

    std::vector<ioring_res> wait() {
      const auto count = io_uring_cq_ready(&ring_);
      if (count > 0) {
        return poll();
      }

      struct io_uring_cqe* entry;
      if (0 == io_uring_wait_cqe(&ring_, &entry)) {
        const ioring_res res{io_uring_cqe_get_data64(entry), static_cast<std::size_t>(entry->res)};
        io_uring_cqe_seen(&ring_, entry);
        return {res};
      } else {
        return {};
      }
    }
  };
} // namespace platform

export namespace fabric::io {
  using ring_t     = platform::ioring;
  using ring_res_t = platform::ioring_res;
} // namespace fabric::io
