// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  io_worker.cppm
 *! \brief
 *!
 */

export module fabric.io.io_worker;

import std;
import reflect;

import fabric.logging;

import fabric.io.rings.uring;
export import fabric.tasks;
export import fabric.io.rings.requests;

export namespace fabric::io {
  class io_worker {
    struct state_t {
      std::unique_ptr<ring_t> ring_{std::make_unique<ring_t>()};
      std::mutex              ring_mutex_{};
    };

    std::shared_ptr<state_t>      state_;
    std::unique_ptr<std::jthread> thread_;

    using make_request_handle_t = task<int>::handle_type;
    using return_slot_t         = std::pair<int, void*>;

    template <typename RequestType>
    struct awaitable_io {
      std::shared_ptr<state_t> state;
      const RequestType&       request;
      return_slot_t            return_slot;

      bool await_ready() const noexcept {
        return false;
      }

      void await_suspend(make_request_handle_t h) noexcept {
        return_slot = {0xFF'FF'FF'FF, h.address()};
        std::unique_lock lk{state->ring_mutex_};
        state->ring_->submit(request, &return_slot);
      }

      int await_resume() const noexcept {
        return return_slot.first;
      }
    };

    io_worker()
        : state_(std::make_shared<state_t>()),
          thread_(
            std::make_unique<std::jthread>(
              [](std::stop_token stop_token, const std::shared_ptr<state_t>& state) {
                while (not stop_token.stop_requested()) {
                  for (const auto& response: state->ring_->wait()) {
                    return_slot_t& return_slot =
                      *static_cast<return_slot_t*>(response.user_data_as_ptr());
                    auto& [res, task_addr] = return_slot;
                    auto handle            = make_request_handle_t::from_address(task_addr);

                    res = response.result;
                    handle.promise().reschedule();
                  }
                }
              },
              state_
            )
          ) {}

  public:
    using sptr = std::shared_ptr<io_worker>;

    static sptr make() {
      return std::shared_ptr<io_worker>(new io_worker());
    }

    template <typename RequestType>
    task<int> make_request(const RequestType& request) {
      int res = co_await awaitable_io<RequestType>{state_, request};
      co_return res;
    }
  };
} // namespace fabric::io
