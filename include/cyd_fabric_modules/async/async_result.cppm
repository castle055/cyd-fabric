// Copyright (c) 2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  async_result.cppm
 *! \brief
 *!
 */

export module fabric.async.result;

export import fabric.tasks;
export import fabric.result;

namespace detail {
  template <typename...>
  struct async_result_t;

  template <>
  struct async_result_t<> {
    using type = fabric::task<>;
  };

  template <typename T>
    requires (not ErrorConcept<T>)
  struct async_result_t<T> {
    using type = fabric::task<T>;
  };

  template <typename T>
    requires ErrorConcept<T>
  struct async_result_t<T> {
    using type = fabric::task<Result<T>>;
  };

  template <typename T, typename... Errors>
    requires(sizeof...(Errors) > 0 and (ErrorConcept<Errors> and ...))
  struct async_result_t<T, Errors...> {
    using type = fabric::task<Result<T, Errors...>>;
  };
} // namespace detail

export template <typename... Args>
using Async = detail::async_result_t<Args...>::type;
