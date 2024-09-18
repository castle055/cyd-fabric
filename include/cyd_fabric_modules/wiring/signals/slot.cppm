// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  slot.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:slot;

import std;

import fabric.ts.packs;

import :types;

export template <typename R, typename... Args>
class fabric::wiring::slot<R(Args...)>: public std::function<R(Args...)> {
public:
  slot()                            = default;

  slot(slot&& func)                 = default;
  slot(const slot& func)            = default;
  slot& operator=(slot&& func)      = default;
  slot& operator=(const slot& func) = default;

  template <typename F>
    requires requires(F f, Args... args) {
      requires(not ts::packs::is_type<slot, std::remove_reference_t<F>>::value);
      { f.operator()(args...) } -> std::convertible_to<R>;
    }
  slot(const F& func)
      : std::function<R(Args...)>(func) {}

  template <typename F>
    requires requires(F f, Args... args) {
      requires(not ts::packs::is_type<slot, std::remove_reference_t<F>>::value);
      { f.operator()(args...) } -> std::same_as<R>;
    }
  slot(F&& func)
      : std::function<R(Args...)>(func) {}

  template <typename F>
  slot(F* instance, R (F::*func)(Args...))
      : std::function<R(Args...)>([instance, func](Args&&... args) {
          return (instance->*func)(args...);
        }) {
    if constexpr (std::derived_from<F, auto_disconnect>) {
      lifetime_bound_ = instance;
    }
  }

  template <typename F>
  slot(F* instance, R (F::*func)(Args...) const)
      : std::function<R(Args...)>([instance, func](Args&&... args) {
          return (instance->*func)(args...);
        }) {
    if constexpr (std::derived_from<F, auto_disconnect>) {
      lifetime_bound_ = instance;
    }
  }

private:
  friend signal<Args...>;

  void* lifetime_bound_ = nullptr;

  bool lifetime_bound() const {
    return lifetime_bound_ != nullptr;
  }

  auto_disconnect* get_auto_disconnect_object() const {
    if (!lifetime_bound())
      return nullptr;
    return static_cast<auto_disconnect*>(lifetime_bound_);
  }

public:
  /**
   * This constructor declaration exists for the purpose of better error reporting.
   *
   * [ERROR] Provided member function does not accept this slot's arguments
   */
  template <typename F, typename... WrongArgs>
  slot(F* instance, R (F::*func)(WrongArgs...)) = delete;

  /**
   * This constructor declaration exists for the purpose of better error reporting.
   *
   * [ERROR] Provided member function does not accept this slot's arguments
   */
  template <typename F, typename... WrongArgs>
  slot(F* instance, R (F::*func)(WrongArgs...) const) = delete;

  /**
   * This constructor declaration exists for the purpose of better error reporting.
   *
   * [ERROR] Provided member function does not belong to the provided object instance
   */
  template <typename T, typename F, typename... WrongArgs>
  slot(T* instance, R (F::*func)(WrongArgs...)) = delete;

  /**
   * This constructor declaration exists for the purpose of better error reporting.
   *
   * [ERROR] Provided member function does not belong to the provided object instance
   */
  template <typename T, typename F, typename... WrongArgs>
  slot(T* instance, R (F::*func)(WrongArgs...) const) = delete;
};
