// Copyright (c) 2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  Error.cppm
 *! \brief
 *!
 */

export module fabric.error;

import std;
import reflect;

import fabric.exception;
import fabric.logging;
import fabric.source_location;

export class Error {
public:
};

export template <typename T>
concept ErrorConcept = std::derived_from<T, Error>;

export template <ErrorConcept E>
class ErrorException: public fabric::exception_base {
public:
  using error_type = E;

  error_type error;

  explicit ErrorException(
    const error_type&             error,
    const unsigned int            skip_frames     = 0,
    const fabric::SourceLocation& source_location = {}
  )
      : exception_base(refl::type_name<E>, "", skip_frames + 1, source_location),
        error(error) {}

  explicit ErrorException(
    error_type&&                  error,
    const unsigned int            skip_frames     = 0,
    const fabric::SourceLocation& source_location = {}
  )
      : exception_base(refl::type_name<E>, "", skip_frames + 1, source_location),
        error(std::forward<error_type>(error)) {}
};

export class InvalidErrorAccessException final: public fabric::exception_base {
public:
  explicit InvalidErrorAccessException(
    const unsigned int skip_frames = 0, const fabric::SourceLocation& source_location = {}
  )
      : exception_base(
          "InvalidErrorAccessException",
          "Result does not contain an error",
          skip_frames + 1,
          source_location
        ) {}
};
