// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  matrix.cppm
 *! \brief 
 *!
 */

export module fabric.linalg:matrix;

import std;
import fabric.memory.multidim_data;

import :vector;

export template<typename T, std::size_t ROWS, std::size_t COLUMNS>
struct mat final: md_buffer_t<T, 2> {
  using scalar = T;

  mat(): md_buffer_t<T, 2>({ROWS, COLUMNS}) {
  }

  vec<T, COLUMNS> operator[](std::size_t row) {
    vec<T, COLUMNS> ret { };
    for (std::size_t col = 0; col < COLUMNS; ++col) {
      ret[col] = (*this)[row, col];
    }
    return ret;
  }

  T &operator[](std::size_t row, std::size_t column) {
    return md_buffer_t<T, 2>::operator[](row, column);
  }

  const T &operator[](std::size_t row, std::size_t column) const {
    return md_buffer_t<T, 2>::operator[](row, column);
  }
};

