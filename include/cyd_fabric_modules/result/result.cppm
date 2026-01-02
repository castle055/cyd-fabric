// Copyright (c) 2025-2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  result.cppm
 *! \brief
 *!
 */

export module fabric.result;

import std;
import reflect;
import packtl;

import fabric.ts.apply;
import fabric.templates.functor_arguments;
import fabric.logging;
import fabric.exception;
export import fabric.error;

template <ErrorConcept E>
class ErrorAdapter {
public:
  using error_type = E;
};


template <typename T>
struct ResultStorage;

template <typename T>
  requires(not std::is_void_v<T>)
struct ResultStorage<T> {
  T value{};
};

template <>
struct ResultStorage<void> {};

export template <typename... Ts>
struct Result {
private:
  using first_T = typename packtl::get_first<packtl::pack<Ts...>>::type;

  static constexpr bool first_T_is_void        = std::is_void_v<first_T>;
  static constexpr bool result_is_void         = first_T_is_void or ErrorConcept<first_T>;
  static constexpr bool first_T_is_return_type = not ErrorConcept<first_T>;

public:
  using result_type         = std::conditional_t<result_is_void, void, first_T>;
  using result_storage_type = ResultStorage<result_type>;
  using error_types         = std::conditional_t<
            first_T_is_return_type,
            typename packtl::remove_first<1, Ts...>::type,
            packtl::pack<Ts...>>;

  static constexpr std::size_t error_type_count   = packtl::get_size<error_types>::value;
  static constexpr bool        can_contain_errors = error_type_count > 0;

  template <ErrorConcept E>
  static constexpr bool can_contain_error = packtl::has_item<E, error_types>::value;

  using error_union = std::conditional_t<
    can_contain_errors,
    typename packtl::swap_pack<error_types, std::variant>::type,
    void>;

private:
  template <ErrorConcept E>
  using without_error_type = packtl::swap_pack<
    typename fabric::ts::with_type<error_types>                           //
    ::template apply_as_pack_w_args<packtl::take_one_out, E>              //
    ::template apply_as_pack_w_args<packtl::prepend, result_storage_type> //
    ::result_pack,
    Result>::type;

  using value_union = std::conditional_t<
    (not can_contain_errors),
    std::variant<std::monostate, ResultStorage<result_type>>,
    std::variant<std::monostate, ResultStorage<result_type>, error_union>>;

private:
  value_union value_{};

public:
  // Default construct
  Result()
    requires (result_is_void or std::is_default_constructible_v<result_type>)
  {
    value_.template emplace<result_storage_type>();
  }

  // construct from value
  template <typename R = result_type>
    requires(not result_is_void and not ErrorConcept<R> and std::convertible_to<R, result_type>)
  Result(const R& ret)
    requires std::is_copy_constructible_v<result_type>
      : value_(result_storage_type{result_type(ret)}) {}

  template <typename R = result_type>
    requires(not result_is_void and not ErrorConcept<R> and std::convertible_to<R, result_type>)
  Result(R&& ret)
      : value_(result_storage_type{result_type(std::move(ret))}) {}

  // construct from error
  template <ErrorConcept E>
    requires can_contain_error<E>
  Result(const E& err) {
    value_.template emplace<error_union>(err);
  }

  template <ErrorConcept E>
    requires can_contain_error<E>
  Result(E&& err) {
    value_.template emplace<error_union>(std::move(err));
  }

  // construct from different Result type
  template <ErrorConcept... Es>
    requires(can_contain_error<Es> and ...)
  Result(Result<result_type, Es...>&& res) {
    if (res.ok()) {
      value_.template emplace<result_storage_type>(
        result_storage_type{result_type(std::move(res.value()))}
      );
    } else {
      std::visit(
        [&]<ErrorConcept E>(const E& err) { value_.template emplace<error_union>(std::move(err)); },
        res.error()
      );
    }
  }

  template <typename R, ErrorConcept... Es>
    requires(
      (can_contain_error<Es> and ...) and (result_is_void or std::is_convertible_v<R, result_type>)
    )
  Result(Result<R, Es...>&& res) {
    if (res.ok()) {
      if constexpr (result_is_void) {
        value_.template emplace<result_storage_type>(result_storage_type{});
      } else {
        value_.template emplace<result_storage_type>(
          result_storage_type{result_type(std::move(res.value()))}
        );
      }
    }
    if (not res.ok()) {
      std::visit(
        [&]<ErrorConcept E>(const E& err) { value_.template emplace<error_union>(std::move(err)); },
        res.error()
      );
    }
  }

  Result(Result<void>&& err)
    requires(result_is_void and error_type_count == 0)
      : value_(result_storage_type{}) {}

  Result(Result<>&& err)
    requires(result_is_void and error_type_count == 0)
      : value_(result_storage_type{}) {}

  // construct from error union
  template <ErrorConcept... Es>
    requires(can_contain_error<Es> and ...)
  Result(std::variant<Es...>&& errors) {
    std::visit(
      [&]<ErrorConcept E>(E&& err) { value_.template emplace<error_union>(std::move(err)); }, errors
    );
  }
  template <ErrorConcept... Es>
    requires(can_contain_error<Es> and ...)
  Result(const std::variant<Es...>& errors) {
    std::visit(
      [&]<ErrorConcept E>(const E& err) { value_.template emplace<error_union>(err); }, errors
    );
  }

  // No Copy
  Result(const Result& other)            = delete;
  Result& operator=(const Result& other) = delete;

  // Default Move
  Result(Result&& other)            = default;
  Result& operator=(Result&& other) = default;

  template <typename Self>
    requires(result_is_void)
  void throw_error(this Self&& self, const fabric::SourceLocation& source_location = {}) {
    std::forward<Self>(self).throw_if_error(source_location);
  }

  template <typename Self>
    requires(not result_is_void)
  auto&& throw_error(this Self&& self, const fabric::SourceLocation& source_location = {}) {
    std::forward<Self>(self).throw_if_error(source_location);
    return std::forward<Self>(self).value();
  }

  template <typename E>
    requires can_contain_error<E>
  auto throw_error(const fabric::SourceLocation& source_location = {}) {
    if (not ok()) {
      const error_union& err = error();
      if (std::holds_alternative<E>(err)) {
        throw ErrorException{std::get<E>(err), 0, source_location};
      }
    }

    if constexpr (packtl::get_size<error_types>::value > 1) {
      return without_error_type<E>{std::move(*this)};
    } else {
      return value();
    }
  }

  template <typename E, typename Handler>
    requires(
      std::same_as<E, typename fabric::first_argument<Handler>::type> and can_contain_error<E> and
      requires(E e, Handler h) { h(e); }
    )
  auto handle_error(Handler&& handler) {
    if (not ok()) {
      const error_union& err = error();
      if (std::holds_alternative<E>(err)) {
        const E& error = std::get<E>(err);
        handler(error);
      }
    }

    if constexpr (packtl::get_size<error_types>::value > 1) {
      return without_error_type<E>{std::move(*this)};
    } else {
      return value();
    }
  }

  template <typename Handler>
    requires(
      can_contain_error<typename fabric::first_argument<Handler>::type> and
      requires(typename fabric::first_argument<Handler>::type e, Handler h) { h(e); }
    )
  auto handle_error(Handler&& handler) {
    using E = fabric::first_argument<Handler>::type;
    return handle_error<E, Handler>(std::forward<Handler>(handler));
  }

  template <class Self>
    requires(not result_is_void)
  auto&& value(this Self&& self, const fabric::SourceLocation& source_location = {}) {
    self.throw_if_error(source_location);
    return std::get<result_storage_type>(std::forward<Self>(self).value_).value;
  }

  template <typename EUnion = error_union>
    requires(can_contain_errors)
  const EUnion& error(const fabric::SourceLocation& source_location = {}) const {
    if (ok()) {
      throw InvalidErrorAccessException{0, source_location};
    }
    return std::get<2>(value_);
  }

  bool ok() const {
    return std::holds_alternative<result_storage_type>(value_);
  }

private:
  void throw_if_error(const fabric::SourceLocation& source_location = {}) const {
    if constexpr (can_contain_errors) {
      if (not ok()) {
        const error_union& err = error();
        std::visit(
          [source_location]<typename E>(E&& error) {
            throw ErrorException{std::forward<E>(error), 0, source_location};
          },
          err
        );
      }
    }
  }
};

export template <>
struct Result<> {
  using result_type                             = void;
  using error_types                             = packtl::pack<>;
  static constexpr std::size_t error_type_count = 0;
  using error_union                             = void;

  // Default construct
  Result() = default;

  // construct from different Result type
  template <typename R>
    requires(not ErrorConcept<R>)
  Result(Result<R>&& err) {}

  // No Copy
  Result(const Result& other)            = delete;
  Result& operator=(const Result& other) = delete;

  // Default Move
  Result(Result&& other)            = default;
  Result& operator=(Result&& other) = default;

  bool ok() const {
    return true;
  }
};

template <typename>
struct is_result: std::false_type {};
template <typename R, ErrorConcept... Errs>
struct is_result<Result<R, Errs...>>: std::true_type {};

template <typename T>
constexpr bool is_result_v = is_result<T>::value;

export template <typename T>
concept ResultConcept = is_result_v<T>;

template <typename...>
struct map_result_type_t;

template <typename T, typename R, ErrorConcept... Errs>
  requires(not ErrorConcept<R>)
struct map_result_type_t<Result<R, Errs...>, T> {
  using type = Result<T, Errs...>;
};

template <typename T, ErrorConcept... Errs>
struct map_result_type_t<Result<Errs...>, T> {
  using type = Result<T, Errs...>;
};

export template <typename... Args>
using map_result_type = map_result_type_t<Args...>::type;
