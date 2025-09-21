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

namespace fabric::tasks::detail {
  thread_local executor* current_executor_{nullptr};
}

export namespace fabric::tasks {
  class executor_thread_base {
  public:
    virtual ~executor_thread_base() = default;

    virtual void                        join()                       = 0;
    virtual void                        request_stop()               = 0;
    virtual void                        keep_alive(bool ka)          = 0;
    virtual std::thread::id             id() const                   = 0;
    virtual std::shared_ptr<schedule_t> get_schedule()               = 0;
    virtual bool                        is_running() const           = 0;
    virtual void                        set_executor(executor* exec) = 0;
  };

  class executor_thread_t: public executor_thread_base {
    const std::size_t           id_{executor_id++};
    executor*                   executor_{nullptr};
    std::shared_ptr<schedule_t> schedule_{std::make_shared<tasks::schedule_t>()};
    std::jthread                thread_;
    std::latch                  join_wait_{1};
    std::atomic_int             keep_alive_{0};
    std::atomic_flag            ready_{false};
    std::atomic_flag            dead_{false}; // thread has stopped and has been joined

  public:
    explicit executor_thread_t()
        : thread_(
            [this](std::stop_token stop_token, const std::shared_ptr<schedule_t>& sched) {
              fabric::set_thread_name(std::format("executor-{}", id_));
              ready_.wait(false);
              detail::current_executor_ = executor_;
              while (true) {
                if (stop_token.stop_requested() and sched->empty() and keep_alive_.load() == 0) {
                  break;
                }
                sched->wait();
                sched->run_all();
              }
              join_wait_.count_down();
            },
            schedule_
          ) {}

    ~executor_thread_t() override {
      if (not dead_.test()) {
        join();
      }
    }

    void join() override {
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

    void request_stop() override {
      thread_.request_stop();
      schedule_->notify();
    }

    void keep_alive(bool ka) override {
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

    std::thread::id id() const override {
      return thread_.get_id();
    }

    std::shared_ptr<schedule_t> get_schedule() override {
      return schedule_;
    }

    bool is_running() const override {
      return not dead_.test();
    }

    void set_executor(executor* exec) override {
      executor_ = exec;
      ready_.test_and_set();
      ready_.notify_all();
    }
  };


  class main_executor_thread_t: public executor_thread_base {
    const std::size_t           id_{executor_id++};
    executor*                   executor_{nullptr};
    std::shared_ptr<schedule_t> schedule_{std::make_shared<schedule_t>()};
    std::latch                  join_wait_{1};
    std::atomic_int             keep_alive_{0};
    std::atomic_flag            dead_{false}; // thread has stopped and has been joined
    std::stop_source            stop_source_{};
    std::thread::id             thread_id_{};

  public:
    explicit main_executor_thread_t() = default;

    void run() {
      thread_id_                 = std::this_thread::get_id();
      detail::current_executor_  = executor_;
      std::stop_token stop_token = stop_source_.get_token();
      while (true) {
        if (schedule_->empty() and keep_alive_.load() == 0) {
          break;
        }
        schedule_->wait();
        schedule_->run_all();
      }
      join_wait_.count_down();
    }

    ~main_executor_thread_t() override {
      if (not dead_.test()) {
        join();
      }
    }

    void join() override {
      if (dead_.test_and_set()) {
        LOG::print{WARN}("Thread already stopped, can't join again.");
        return;
      }
      LOG::print{DEBUG}("Joining executor thread...");

      stop_source_.request_stop();
      schedule_->notify();
      join_wait_.wait();

      LOG::print{DEBUG}("Thread joined");
    }

    void request_stop() override {
      stop_source_.request_stop();
      schedule_->notify();
    }

    void keep_alive(bool ka) override {
      if (ka) {
        keep_alive_.fetch_add(1);
        LOG::print{DEBUG}("(+1): {}", keep_alive_.load());
      } else {
        keep_alive_.fetch_sub(1);
        keep_alive_.notify_all();
        schedule_->notify();
        LOG::print{DEBUG}("(-1): {}", keep_alive_.load());
      }
    }

    std::thread::id id() const override {
      return thread_id_;
    }

    std::shared_ptr<schedule_t> get_schedule() override {
      return schedule_;
    }

    bool is_running() const override {
      return not dead_.test();
    }

    void set_executor(executor* exec) override {
      executor_ = exec;
    }
  };

  class executor {
    std::weak_ptr<executor> self_;

    std::shared_ptr<task_context>         spawn_context_{task_context::make()};
    std::shared_ptr<executor_thread_base> thread_;
    std::shared_ptr<schedule_t>           schedule_;

    executor()
        : thread_(std::make_shared<executor_thread_t>()),
          schedule_(thread_->get_schedule()) {
      thread_->set_executor(this);
    }

    explicit executor(const std::shared_ptr<executor_thread_base>& thread)
        : thread_(thread),
          schedule_(thread_->get_schedule()) {
      thread_->set_executor(this);
    }

  public:
    using sptr = std::shared_ptr<executor>;

    ~executor() {
      request_stop();
      join();
    }

    static sptr make() {
      auto ptr   = std::shared_ptr<executor>(new executor());
      ptr->self_ = ptr;
      return ptr;
    }

    static sptr make(const std::shared_ptr<executor_thread_base>& thread) {
      auto ptr   = std::shared_ptr<executor>(new executor(thread));
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

    bool is_running() const {
      return thread_->is_running();
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
      if (handle.done()) {
        return;
      }
      handle.promise().set_executor(self_);
      if (due <= clock::now()) {
        schedule_->enqueue(handle);
      } else {
        schedule_->enqueue_delayed(due, handle);
      }
    }

    //! \brief Enqueue an already instantiated task handle
    void schedule_handle(const task_handle<>& handle, time_point due = clock::now() + 0ms) const {
      if (handle.done()) {
        return;
      }
      if (due <= clock::now()) {
        schedule_->enqueue(handle);
      } else {
        schedule_->enqueue_delayed(due, handle);
      }
    }

    template <typename R>
    void schedule(const task<R>& handle, time_point due = clock::now() + 0ms) const {
      schedule_handle(handle.get_handle(), due);
    }

    template <typename R>
    void schedule(const task<R>& handle, duration delay) const {
      schedule_handle(handle.get_handle(), clock::now() + delay);
    }

    template <typename R>
    task<R> schedule(task<R>&& handle, time_point due = clock::now() + 0ms) const {
      schedule_handle(handle.get_handle(), due);
      return handle;
    }

    template <typename R>
    task<R> schedule(task<R>&& handle, duration delay) const {
      schedule_handle(handle.get_handle(), clock::now() + delay);
      return handle;
    }

    //   //! \brief Enqueue anything that is a coroutine, so anything that returns `task<>`
    //   template <
    //     typename C,
    //     typename... Args,
    //     typename R = typename std::invoke_result_t<C, Args...>::return_type>
    //     requires requires(const C& c, Args&&... args) {
    //       { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
    //     }
    //   task<R> schedule(time_point due, C& coroutine, Args&&... args) const {
    //     auto t = coroutine(std::forward<Args>(args)...);
    //     schedule_handle(t.get_handle(), due);
    //     return t;
    //   }
    //
    //   template <
    //     typename C,
    //     typename... Args,
    //     typename R = typename std::invoke_result_t<C, Args...>::return_type>
    //     requires requires(const C& c, Args&&... args) {
    //       { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
    //     }
    //   task<R> schedule(duration delay, C& coroutine, Args&&... args) const {
    //     return schedule(clock::now() + delay, coroutine, std::forward<Args>(args)...);
    //   }
    //
    //   template <
    //     typename C,
    //     typename... Args,
    //     typename R = typename std::invoke_result_t<C, Args...>::return_type>
    //     requires requires(const C& c, Args&&... args) {
    //       { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
    //     }
    //   task<R> schedule(C& coroutine, Args&&... args) const {
    //     return schedule(clock::now(), coroutine, std::forward<Args>(args)...);
    //   }
    //
    // private:
    //   template <typename C, typename... Args>
    //   static auto schedule_helper(C coro, Args... args)
    //     -> task<typename decltype(coro(args...))::return_type> {
    //     if constexpr (std::is_void_v<typename decltype(coro(args...))::return_type>) {
    //       co_await coro(args...);
    //       co_return;
    //     } else {
    //       co_return co_await coro(args...);
    //     }
    //   }
    //
    // public:
    //   template <
    //     typename C,
    //     typename... Args,
    //     typename R = typename std::invoke_result_t<C, Args...>::return_type>
    //     requires requires(C&& c, Args&&... args) {
    //       { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
    //     } and (not(std::is_lvalue_reference_v<C>))
    //   task<R> schedule(time_point due, C&& coroutine, Args&&... args) const {
    //     auto t = schedule_helper(std::forward<C>(coroutine), std::forward<Args>(args)...);
    //     schedule_handle(t.get_handle(), due);
    //     return t;
    //   }
    //
    //   template <
    //     typename C,
    //     typename... Args,
    //     typename R = typename std::invoke_result_t<C, Args...>::return_type>
    //     requires requires(C&& c, Args&&... args) {
    //       { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
    //     } and (not(std::is_lvalue_reference_v<C>))
    //   task<R> schedule(duration delay, C&& coroutine, Args&&... args) const {
    //     return schedule(clock::now() + delay, std::move(coroutine), std::forward<Args>(args)...);
    //   }
    //
    //   template <
    //     typename C,
    //     typename... Args,
    //     typename R = typename std::invoke_result_t<C, Args...>::return_type>
    //     requires requires(C&& c, Args&&... args) {
    //       { c(std::forward<Args>(args)...) } -> std::convertible_to<task<R>>;
    //     } and (not(std::is_lvalue_reference_v<C>))
    //   task<R> schedule(C&& coroutine, Args&&... args) const {
    //     return schedule(clock::now(), std::move(coroutine), std::forward<Args>(args)...);
    //   }
  };


  void continuation_list_t::await_suspend(task_handle<> h) noexcept {
    // Continuations might destroy the frame so copy/move what's needed first
    bool is_detached = detached;
    auto conts       = std::move(continuations);

    if (nullptr != current_executor) {
      for (const auto& [exec, handle]: conts) {
        exec->schedule_handle(handle);
      }
    }
    if (is_detached) {
      h.destroy();
    }
  }
} // namespace fabric::tasks

export namespace fabric::this_executor {
  tasks::executor& get() {
    return *tasks::detail::current_executor_;
  }
} // namespace fabric::this_executor
