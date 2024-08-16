// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  integrators.cppm
 *! \brief 
 *!
 */

export module fabric.linalg:integrators;

import std;

import :concepts;
import :vector;
import :matrix;

export template<typename T>
struct integrators {
  integrators() = delete;

  template<std::size_t TS, std::size_t US>
  static inline std::pair<vec<T, TS>, mat<T, TS, US>> euler_exp(
    Fun<vec<T, US>(T, vec<T, US>)> auto &&dudt,
    vec<T, TS> t_span,
    vec<T, US> u_0
  ) {
    mat<T, TS, US> u { };
    for (std::size_t col = 0; col < US; ++col) {
      u[0, col] = u_0[col];
    }

    std::size_t step = 1;

    for (; step < TS; ++step) {
      auto du  = dudt(t_span[step], u[step - 1]);
      auto u_1 = u[step - 1] + du * (t_span[step] - t_span[step - 1]);
      for (std::size_t col = 0; col < US; ++col) {
        u[step, col] = u_1[col];
      }
    }

    return {t_span, u};
  }
};

