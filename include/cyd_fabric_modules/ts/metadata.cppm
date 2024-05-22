// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  metadata.cppm
 *! \brief 
 *!
 */

export module fabric.ts.metadata;
export import fabric.ts.apply;

export namespace cyd::fabric::ts {
  template<typename T>
  struct metadata;

  template<typename T>
  struct metadata: with_type<T> {
  };

  template<template <typename...> typename T, typename... Args>
  struct metadata<T<Args...>>: with_type<T<Args...>> {
  };
}
