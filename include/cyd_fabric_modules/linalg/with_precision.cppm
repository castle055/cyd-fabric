// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  with_precision.cppm
 *! \brief 
 *!
 */

export module fabric.linalg:with_precision;

import std;

import :vector;
import :matrix;

import :integrators;

export template<typename T>
struct with_precision {
  with_precision() = delete;

  using scalar = T;

  template<std::size_t SIZE>
  using vec = vec<T, SIZE>;

  template<std::size_t ROWS, std::size_t COLUMNS>
  using mat = mat<T, ROWS, COLUMNS>;

  using integrators = integrators<T>;

  template<std::size_t SIZE>
  static inline vec<SIZE> linspace(T start, T end) {
    vec<SIZE> ret { };
    T step = (end - start) / SIZE;
    T acc  = start;
    for (std::size_t i = 0; i < SIZE; ++i, acc += step) {
      ret[i] = acc;
    }
    return ret;
  }

  template<std::size_t SIZE, typename F>
  static inline vec<SIZE> transform(const vec<SIZE> &v, F &&fun) {
    vec<SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = fun(v[i]);
    }
    return ret;
  }

  template<std::size_t SIZE>
  static inline vec<SIZE> sin(const vec<SIZE> &v) {
    return transform(v, [](const T &it) { return std::sin(it); });
  }
};
