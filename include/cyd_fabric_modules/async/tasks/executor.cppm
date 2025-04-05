// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:executor;

import std;
import reflect;

export import :types;
export import :contexts;
export import :task;
export import :schedule;

import fabric.concurrent_queue;
import fabric.thread_name;

using namespace std::chrono_literals;

std::size_t executor_id{0};

export namespace fabric::tasks {
  class executor_thread_t {
    const std::size_t           id_{executor_id++};
    std::shared_ptr<schedule_t> schedule_{std::make_shared<tasks::schedule_t>()};
    std::jthread                thread_;
    std::latch                  join_wait_{1};
    std::atomic_int             keep_alive_{0};
    std::atomic_flag            dead_{false}; // thread has stopped and has been joined

  public:
    explicit executor_thread_t(const std::shared_ptr<schedule_t>& schedule)
        : schedule_(schedule),
          thread_(
            [this](std::stop_token stop_token, const std::shared_ptr<schedule_t>& sched) {
              fabric::set_thread_name(std::format("executor-{}", id_));
              while (true) {
                if (stop_token.stop_requested() and sched->empty() and keep_alive_.load() == 0) {
                  break;
                }
                sched->wait();
                sched->run_all();
              }
              join_wait_.count_down();
            },
            schedule
          ) {}

    ~executor_thread_t() {
      if (not dead_.test()) {
        join();
      }
    }

    void join() {
      if (dead_.test_and_set()) {
        LOG::print{WARN}("Thread already stopped, can't join again.");
        return;
      }
      LOG::print{DEBUG}("Joining thread...");

      thread_.request_stop();
      schedule_->notify();
      join_wait_.wait();
      thread_.join();

      LOG::print{DEBUG}("Thread joined");
    }

    void request_stop() {
      thread_.request_stop();
      schedule_->notify();
    }

    void keep_alive(bool ka) {
      if (ka) {
        keep_alive_.fetch_add(1);
        LOG::print{DEBUG}("keep_alive (+1): {}", keep_alive_.load());
      } else {
        keep_alive_.fetch_sub(1);
        keep_alive_.notify_all();
        schedule_->notify();
        LOG::print{DEBUG}("keep_alive (-1): {}", keep_alive_.load());
      }
    }

    std::thread::id id() const {
      return thread_.get_id();
    }
  };

  class executor {
    std::weak_ptr<executor> self_;

    std::shared_ptr<task_context>      spawn_context_{task_context::make()};
    std::shared_ptr<schedule_t>        schedule_{std::make_shared<schedule_t>()};
    std::unique_ptr<executor_thread_t> thread_{std::make_unique<executor_thread_t>(schedule_)};

    executor() = default;

  public:
    using sptr = std::shared_ptr<executor>;

    static sptr make() {
      auto ptr   = std::shared_ptr<executor>(new executor());
      ptr->self_ = ptr;
      return ptr;
    }

    void join() {
      thread_->join();
    }

    void request_stop() {
      thread_->request_stop();
      schedule_->notify();
    }

    void keep_alive(bool ka) {
      thread_->keep_alive(ka);
    }

    sptr as_sptr() const {
      return self_.lock();
    }

    std::shared_ptr<schedule_t> get_schedule() const {
      return schedule_;
    }

    std::shared_ptr<task_context> get_spawn_context() const {
      return spawn_context_;
    }

    void set_spawn_context(const std::shared_ptr<task_context>& ctx) {
      spawn_context_ = ctx;
    }

    //! \brief Enqueue an already instantiated task handle
    template <typename P>
    void schedule_handle(const task_handle<P>& handle, time_point due = clock::now() + 0ms) const {
      handle.promise().set_executor(self_);
      if (due <= clock::now()) {
        schedule_->enqueue(handle);
      } else {
        schedule_->enqueue_delayed(due, handle);
      }
    }
    //! \brief Enqueue an already instantiated task
    template <typename R>
    task<R> schedule(task<R>&& handle, time_point due = clock::now() + 0ms) const {
      schedule_handle(handle.get_handle(), due);
      return std::move(handle);
    }

    //! \brief Enqueue an already instantiated task
    template <typename R>
    task<R>& schedule(task<R>& handle, time_point due = clock::now() + 0ms) const {
      schedule_handle(handle.get_handle(), due);
      return handle;
    }

    //! \brief Enqueue an already instantiated task
    template <typename R>
    task<R> schedule(task<R>&& handle, duration delay) const {
      schedule_handle(handle.get_handle(), clock::now() + delay);
      return std::move(handle);
    }

    //! \brief Enqueue an already instantiated task
    template <typename R>
    task<R>& schedule(task<R>& handle, duration delay) const {
      schedule_handle(handle.get_handle(), clock::now() + delay);
      return handle;
    }

    //! \brief Enqueue anything that is a coroutine, so anything that returns `task<>`
    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
      requires requires(const C& c, Args&&... args) {
        { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
      }
    task<R> schedule(time_point due, C& coroutine, Args&&... args) const {
      auto t = coroutine(std::forward<Args>(args)...);
      schedule_handle(t.get_handle(), due);
      return t;
    }

    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
      requires requires(const C& c, Args&&... args) {
        { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
      }
    task<R> schedule(duration delay, C& coroutine, Args&&... args) const {
      return schedule(clock::now() + delay, coroutine, std::forward<Args>(args)...);
    }

    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
      requires requires(const C& c, Args&&... args) {
        { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
      }
    task<R> schedule(C& coroutine, Args&&... args) const {
      return schedule(clock::now(), coroutine, std::forward<Args>(args)...);
    }

  private:
    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
    static task<R> schedule_helper(C coro, Args&&... args) {
      if constexpr (std::is_void_v<R>) {
        co_await coro(std::forward<Args>(args)...);
        co_return;
      } else {
        co_return co_await coro(std::forward<Args>(args)...);
      }
    }

  public:
    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
      requires requires(C&& c, Args&&... args) {
        { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
      } and (not(std::is_lvalue_reference_v<C>))
    task<R> schedule(time_point due, C&& coroutine, Args&&... args) const {
      auto t = schedule_helper(std::move(coroutine), std::forward<Args>(args)...);
      schedule_handle(t.get_handle(), due);
      return t;
    }

    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
      requires requires(C&& c, Args&&... args) {
        { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
      } and (not(std::is_lvalue_reference_v<C>))
    task<R> schedule(duration delay, C&& coroutine, Args&&... args) const {
      return schedule(clock::now() + delay, std::move(coroutine), std::forward<Args>(args)...);
    }

    template <
      typename C,
      typename... Args,
      typename R = typename std::invoke_result_t<C, Args...>::return_type>
      requires requires(C&& c, Args&&... args) {
        { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
      } and (not(std::is_lvalue_reference_v<C>))
    task<R> schedule(C&& coroutine, Args&&... args) const {
      return schedule(clock::now(), std::move(coroutine), std::forward<Args>(args)...);
    }
  };
} // namespace fabric::tasks
