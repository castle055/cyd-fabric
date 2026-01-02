// Copyright (c) 2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 8/15/24.
//

#include "gtest/gtest.h"

#define STATIC_EXPECT_TYPE(TYPE, ...) static_assert(std::same_as<TYPE, __VA_ARGS__>)
#define STATIC_EXPECT_EQ(TYPE, ...)   static_assert(TYPE == (__VA_ARGS__))

import fabric.logging;
import fabric.result;
import packtl;

// Static checks
template <typename R>
concept can_default_construct = std::is_default_constructible_v<R>;
template <typename R>
concept can_copy_construct = std::is_copy_constructible_v<R>;
template <typename R>
concept can_move_construct = std::is_move_constructible_v<R>;
template <typename R, typename... Args>
concept can_construct_from = requires(Args&&... args) { R{std::forward<Args>(args)...}; };

// Test subjects
struct TestValueType {
  int val{0};

  TestValueType() = default;
  TestValueType(int v)
      : val(v) {}

  TestValueType(const TestValueType&)            = default;
  TestValueType& operator=(const TestValueType&) = default;

  TestValueType(TestValueType&& other) noexcept
      : val{other.val} {
    other.val = 0;
  }
  TestValueType& operator=(TestValueType&& other) noexcept {
    val       = other.val;
    other.val = 0;
    return *this;
  }
};
struct TestErrorA: Error {
  int val{0};

  TestErrorA() = default;
  TestErrorA(int v)
      : val(v) {}

  TestErrorA(const TestErrorA&)            = default;
  TestErrorA& operator=(const TestErrorA&) = default;

  TestErrorA(TestErrorA&& other) noexcept
      : val{other.val} {
    other.val = 0;
  }
  TestErrorA& operator=(TestErrorA&& other) noexcept {
    val       = other.val;
    other.val = 0;
    return *this;
  }
};
struct TestErrorB: Error {
  int val{0};
};

struct WrongTestValueType {};
struct WrongTestError: Error {};

struct DerivedTestValueType: TestValueType {};
struct MoveOnlyTestValueType {
  MoveOnlyTestValueType(MoveOnlyTestValueType&&)                 = default;
  MoveOnlyTestValueType& operator=(MoveOnlyTestValueType&&)      = default;
  MoveOnlyTestValueType(const MoveOnlyTestValueType&)            = delete;
  MoveOnlyTestValueType& operator=(const MoveOnlyTestValueType&) = delete;
};
struct CopyOnlyTestValueType {
  CopyOnlyTestValueType(CopyOnlyTestValueType&&)                 = delete;
  CopyOnlyTestValueType& operator=(CopyOnlyTestValueType&&)      = delete;
  CopyOnlyTestValueType(const CopyOnlyTestValueType&)            = default;
  CopyOnlyTestValueType& operator=(const CopyOnlyTestValueType&) = default;
};
struct NoDefaultTestValueType {
  NoDefaultTestValueType() = delete;
};

namespace results {
  using WTypeWError = Result<TestValueType, TestErrorA, TestErrorB>;
  using WType       = Result<TestValueType>;
  using VoidWError  = Result<void, TestErrorA, TestErrorB>;
  using WError      = Result<TestErrorA, TestErrorB>;
  using Void        = Result<void>;
  using Nothing     = Result<>;
} // namespace results

// [Compile-time Tests]

// Type deduction
STATIC_EXPECT_TYPE(TestValueType, results::WTypeWError::result_type);
STATIC_EXPECT_TYPE(TestValueType, results::WType::result_type);
STATIC_EXPECT_TYPE(void, results::VoidWError::result_type);
STATIC_EXPECT_TYPE(void, results::WError::result_type);
STATIC_EXPECT_TYPE(void, results::Void::result_type);
STATIC_EXPECT_TYPE(void, results::Nothing::result_type);

using test_error_pack = packtl::pack<TestErrorA, TestErrorB>;
using empty_pack      = packtl::pack<>;
STATIC_EXPECT_TYPE(test_error_pack, results::WTypeWError::error_types);
STATIC_EXPECT_TYPE(test_error_pack, results::VoidWError::error_types);
STATIC_EXPECT_TYPE(test_error_pack, results::WError::error_types);
STATIC_EXPECT_TYPE(empty_pack, results::WType::error_types);
STATIC_EXPECT_TYPE(empty_pack, results::Void::error_types);
STATIC_EXPECT_TYPE(empty_pack, results::Nothing::error_types);

STATIC_EXPECT_EQ(2, results::WTypeWError::error_type_count);
STATIC_EXPECT_EQ(2, results::VoidWError::error_type_count);
STATIC_EXPECT_EQ(2, results::WError::error_type_count);
STATIC_EXPECT_EQ(0, results::WType::error_type_count);
STATIC_EXPECT_EQ(0, results::Void::error_type_count);
STATIC_EXPECT_EQ(0, results::Nothing::error_type_count);

using test_error_union = std::variant<TestErrorA, TestErrorB>;
STATIC_EXPECT_TYPE(test_error_union, results::WTypeWError::error_union);
STATIC_EXPECT_TYPE(test_error_union, results::VoidWError::error_union);
STATIC_EXPECT_TYPE(test_error_union, results::WError::error_union);
STATIC_EXPECT_TYPE(void, results::WType::error_union);
STATIC_EXPECT_TYPE(void, results::Void::error_union);
STATIC_EXPECT_TYPE(void, results::Nothing::error_union);

// map_result_type
STATIC_EXPECT_TYPE(
  Result<WrongTestValueType, TestErrorA>,
  map_result_type<Result<TestValueType, TestErrorA>, WrongTestValueType>
);
STATIC_EXPECT_TYPE(
  Result<WrongTestValueType, TestErrorA>, map_result_type<Result<TestErrorA>, WrongTestValueType>
);

// Default/Move/Copy Construction
static_assert(can_default_construct<results::WTypeWError>);
static_assert(can_default_construct<results::VoidWError>);
static_assert(can_default_construct<results::WError>);
static_assert(can_default_construct<results::WType>);
static_assert(can_default_construct<results::Void>);
static_assert(can_default_construct<results::Nothing>);
static_assert(not can_default_construct<Result<NoDefaultTestValueType>>);

static_assert(can_move_construct<results::WTypeWError>);
static_assert(can_move_construct<results::VoidWError>);
static_assert(can_move_construct<results::WError>);
static_assert(can_move_construct<results::WType>);
static_assert(can_move_construct<results::Void>);
static_assert(can_move_construct<results::Nothing>);

static_assert(not can_copy_construct<results::WTypeWError>);
static_assert(not can_copy_construct<results::VoidWError>);
static_assert(not can_copy_construct<results::WError>);
static_assert(not can_copy_construct<results::WType>);
static_assert(not can_copy_construct<results::Void>);
static_assert(not can_copy_construct<results::Nothing>);

// Construction from value or error
static_assert(can_construct_from<results::WTypeWError, TestValueType>);
static_assert(can_construct_from<results::WTypeWError, DerivedTestValueType>);
static_assert(can_construct_from<results::WTypeWError, TestErrorA>);
static_assert(can_construct_from<results::WTypeWError, TestErrorB>);
static_assert(not can_construct_from<results::WTypeWError, WrongTestValueType>);
static_assert(not can_construct_from<results::WTypeWError, WrongTestError>);

static_assert(can_construct_from<results::VoidWError, TestErrorA>);
static_assert(can_construct_from<results::VoidWError, TestErrorB>);
static_assert(not can_construct_from<results::VoidWError, WrongTestValueType>);
static_assert(not can_construct_from<results::VoidWError, WrongTestError>);

static_assert(can_construct_from<results::WError, TestErrorA>);
static_assert(can_construct_from<results::WError, TestErrorB>);
static_assert(not can_construct_from<results::WError, WrongTestValueType>);
static_assert(not can_construct_from<results::WError, WrongTestError>);

static_assert(can_construct_from<results::WType, TestValueType>);
static_assert(not can_construct_from<results::WType, WrongTestValueType>);
static_assert(not can_construct_from<results::WType, WrongTestError>);

static_assert(not can_construct_from<results::Void, WrongTestValueType>);
static_assert(not can_construct_from<results::Void, WrongTestError>);

static_assert(not can_construct_from<results::Nothing, WrongTestValueType>);
static_assert(not can_construct_from<results::Nothing, WrongTestError>);

static_assert(not can_construct_from<Result<DerivedTestValueType>, TestValueType>);
static_assert(not can_construct_from<Result<MoveOnlyTestValueType>, MoveOnlyTestValueType&>);
static_assert(can_construct_from<Result<MoveOnlyTestValueType>, MoveOnlyTestValueType&&>);
static_assert(can_construct_from<Result<CopyOnlyTestValueType>, CopyOnlyTestValueType&>);
static_assert(not can_construct_from<Result<CopyOnlyTestValueType>, CopyOnlyTestValueType&&>);

static_assert(not can_construct_from<Result<>, WrongTestValueType>);
static_assert(not can_construct_from<Result<void>, WrongTestValueType>);

// Construction from different Result type
static_assert(can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              Result<TestValueType, TestErrorB, TestErrorA>>);
static_assert(can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              Result<TestValueType, TestErrorB>>);
static_assert(can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              Result<TestValueType, TestErrorA>>);
static_assert(
  not can_construct_from<Result<TestValueType, TestErrorA>, Result<TestValueType, WrongTestError>>
);
static_assert(not can_construct_from<Result<TestValueType, TestErrorA>, Result<TestErrorA>>);
static_assert(not can_construct_from<Result<TestValueType>, Result<TestValueType, WrongTestError>>);
static_assert(not can_construct_from<Result<TestValueType>, Result<WrongTestValueType>>);
static_assert(not can_construct_from<Result<TestValueType>, Result<WrongTestError>>);
static_assert(not can_construct_from<Result<TestValueType>, Result<void>>);
static_assert(not can_construct_from<Result<TestValueType>, Result<>>);

static_assert(
  can_construct_from<Result<void, TestErrorA, TestErrorB>, Result<WrongTestValueType, TestErrorB>>
);
static_assert(
  can_construct_from<Result<void, TestErrorA, TestErrorB>, Result<WrongTestValueType, TestErrorA>>
);
static_assert(can_construct_from<Result<void>, Result<WrongTestValueType>>);
static_assert(can_construct_from<Result<void>, Result<void>>);
static_assert(can_construct_from<Result<void>, Result<>>);
static_assert(
  not can_construct_from<Result<void, TestErrorA>, Result<WrongTestValueType, WrongTestError>>
);
static_assert(not can_construct_from<Result<void>, Result<WrongTestValueType, WrongTestError>>);

static_assert(
  can_construct_from<Result<TestErrorA, TestErrorB>, Result<WrongTestValueType, TestErrorB>>
);
static_assert(
  can_construct_from<Result<TestErrorA, TestErrorB>, Result<WrongTestValueType, TestErrorA>>
);
static_assert(can_construct_from<Result<>, Result<WrongTestValueType>>);
static_assert(can_construct_from<Result<>, Result<void>>);
static_assert(can_construct_from<Result<>, Result<>>);
static_assert(not can_construct_from<Result<>, Result<WrongTestError>>);
static_assert(not can_construct_from<Result<>, Result<WrongTestValueType, WrongTestError>>);
static_assert(
  not can_construct_from<Result<TestErrorA>, Result<WrongTestValueType, WrongTestError>>
);
static_assert(not can_construct_from<Result<>, Result<WrongTestValueType, WrongTestError>>);

static_assert(can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              Result<TestValueType, TestErrorA>&&>);
static_assert(not can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              Result<TestValueType, TestErrorA>&>);

static_assert(can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              Result<DerivedTestValueType, TestErrorA>>);
static_assert(not can_construct_from<
              Result<DerivedTestValueType, TestErrorA, TestErrorB>,
              Result<TestValueType, TestErrorA>>);

static_assert(can_construct_from<
              Result<MoveOnlyTestValueType, TestErrorA, TestErrorB>,
              Result<MoveOnlyTestValueType, TestErrorA>&&>);
static_assert(can_construct_from<
              Result<CopyOnlyTestValueType, TestErrorA, TestErrorB>,
              Result<CopyOnlyTestValueType, TestErrorA>&&>);

// Construction from error union
static_assert(can_construct_from<
              Result<TestValueType, TestErrorA, TestErrorB>,
              std::variant<TestErrorA, TestErrorB>>);
static_assert(not can_construct_from<//
              Result<TestValueType, TestErrorA>,
              std::variant<TestErrorA, TestErrorB>>);

// [Run-time Tests]

TEST(Construction, Default) {
  EXPECT_TRUE((Result<TestValueType, TestErrorA>{}.ok()));
  EXPECT_TRUE((Result<TestValueType>{}.ok()));
  EXPECT_TRUE((Result<TestErrorA>{}.ok()));
}

TEST(Construction, ValueCopy) {
  const TestValueType               value{123};
  Result<TestValueType, TestErrorA> result{value};
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.value().val, 123);
}

TEST(Construction, ValueMove) {
  TestValueType                     value{123};
  Result<TestValueType, TestErrorA> result{std::move(value)};
  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.value().val, 123);
  EXPECT_EQ(value.val, 0);
}

TEST(Construction, ErrorCopy) {
  const TestErrorA                              err{123};
  Result<TestValueType, TestErrorA, TestErrorB> result{err};
  EXPECT_FALSE(result.ok());
  auto& error = result.error();
  EXPECT_TRUE(std::holds_alternative<TestErrorA>(error));
  EXPECT_EQ(std::get<TestErrorA>(error).val, 123);
}

TEST(Construction, ErrorMove) {
  TestErrorA                                    err{123};
  Result<TestValueType, TestErrorA, TestErrorB> result{std::move(err)};
  EXPECT_FALSE(result.ok());
  auto& error = result.error();
  EXPECT_TRUE(std::holds_alternative<TestErrorA>(error));
  EXPECT_EQ(std::get<TestErrorA>(error).val, 123);
  EXPECT_EQ(err.val, 0);
}

TEST(Conversion, ErrorSubset_Value) {
  Result<TestValueType, TestErrorA, TestErrorB> result{Result<TestValueType, TestErrorA>{123}};

  EXPECT_TRUE(result.ok());
  EXPECT_EQ(result.value().val, 123);
}

TEST(Conversion, ErrorSubset_Error) {
  Result<TestValueType, TestErrorA, TestErrorB> result{
    Result<TestValueType, TestErrorA>{TestErrorA{123}}
  };

  EXPECT_FALSE(result.ok());
  auto& error = result.error();
  EXPECT_TRUE(std::holds_alternative<TestErrorA>(error));
  EXPECT_EQ(std::get<TestErrorA>(error).val, 123);
}

TEST(Conversion, IgnoreResult_Value) {
  Result<TestErrorA, TestErrorB> result{Result<TestValueType, TestErrorA>{TestValueType{123}}};
  EXPECT_TRUE(result.ok());
}

TEST(Conversion, IgnoreResult_Error) {
  Result<TestErrorA, TestErrorB> result{Result<TestValueType, TestErrorA>{TestErrorA{123}}};

  EXPECT_FALSE(result.ok());
  auto& error = result.error();
  EXPECT_TRUE(std::holds_alternative<TestErrorA>(error));
  EXPECT_EQ(std::get<TestErrorA>(error).val, 123);
}

TEST(Access, OKTrue) {
  Result<TestValueType, TestErrorA> result{123};
  EXPECT_TRUE(result.ok());
}

TEST(Access, OKFalse) {
  Result<TestValueType, TestErrorA> result{TestErrorA{}};
  EXPECT_FALSE(result.ok());
}

TEST(Access, Value) {
  Result<TestValueType, TestErrorA> result{123};
  EXPECT_NO_THROW(result.value());
  EXPECT_EQ(result.value().val, 123);
}

TEST(Access, InvalidValue) {
  Result<TestValueType, TestErrorA> result{TestErrorA{}};
  EXPECT_THROW(result.value(), ErrorException<TestErrorA>);
}

TEST(Access, Error) {
  Result<TestValueType, TestErrorA> result{TestErrorA{123}};
  EXPECT_NO_THROW(result.error());
  EXPECT_EQ(std::get<TestErrorA>(result.error()).val, 123);
}

TEST(Access, InvalidError) {
  Result<TestValueType, TestErrorA> result{123};
  EXPECT_THROW(result.error(), InvalidErrorAccessException);
}
