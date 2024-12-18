// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FABRIC_COROUTINES_H
#define FABRIC_COROUTINES_H

#include "../testing/test_enabled.h"

#include <coroutine>
#include <future>
#include <optional>

namespace cyd::fabric::async {
    template<typename Ret>
    struct async;
    template<typename T = void>
    using async_handle = std::coroutine_handle<T>;
    
    struct continuation {
      std::optional<async_handle<>> cont;
      bool await_ready() const noexcept {return false;} /// Always suspend!
      
      async_handle<> await_suspend(async_handle<> h) noexcept {
        if (cont.has_value()) {
          return cont.value();
        } else {
          return std::noop_coroutine();//h;
        }
      }
      void await_resume() const noexcept { }
    };
    
    template<typename Ret = void>
    struct async {
    private:
      std::shared_future<Ret> future_;
    public:
      using return_type = Ret;
      struct promise_type;
      async_handle<promise_type> h_;
      
      async(async_handle<promise_type> &&h__, std::shared_future<Ret> &future__)
        : future_(future__), h_(h__) { }
      
      struct promise_type {
        std::promise<Ret> value_promise_ {};
        std::shared_future<Ret> future_ = value_promise_.get_future().share();
        std::optional<async_handle<>> cont = std::nullopt;
        //std::deque<async_handle<>> future_continuations {};
        
        async get_return_object() {
          return async(
            async_handle<promise_type>::from_promise(*this),
            future_
          );
        }
        std::suspend_always initial_suspend() {return {};}
        continuation final_suspend() noexcept {return {cont};}
        void unhandled_exception() { }
        
        std::suspend_always yield_value(bool a) {
          //value_promise_.set_value(value);
          return {};
        }
        
        void return_value(Ret value) {
          value_promise_.set_value(value);
        }
      };
      
      
      operator async_handle<promise_type>() const {return h_;}
      
      struct result_t {
        Ret result;
        bool is_done;
      };
      result_t operator()() {
        return {
          .result = future_.get(),
          .is_done = false,
        };
      }
      
      bool await_ready() const noexcept {return h_.done();}
      
      async_handle<promise_type> await_suspend(async_handle<> h) {
        //auto e = executor::from_this_thread();
        //if (e.has_value()) {
        //  e.value()->queue(h);
        //}
        h_.promise().cont = std::optional {h};
        return h_;
      }
      Ret await_resume() {
        //while (!h_.done()) {
        //  h_.resume();
        //}
        return future_.get();
      }
      
      //template<typename T>
      //struct async_chain {
      //  async<Ret> prev;
      //  async_chain(async<Ret> prev_): prev(prev_) { }
      //
      //  template<typename C, typename R = decltype(std::declval<C>()(std::declval<T>()))::return_type>
      //  async_chain<R> then(C &&coroutine) {
      //    prev.h_.promise().cont = [=](T ret) {
      //      auto a = coroutine(ret);
      //      return a.h_;
      //    };
      //  }
      //};
      //
      //template<typename C, typename R = decltype(std::declval<C>()(std::declval<Ret>()))::return_type>
      //async_chain<R> then(C &&coroutine) {
      //  h_.promise().cont = [=](Ret ret) {
      //    auto a = coroutine(ret);
      //    return a.h_;
      //  };
      //}
    };
}

#endif //FABRIC_COROUTINES_H
