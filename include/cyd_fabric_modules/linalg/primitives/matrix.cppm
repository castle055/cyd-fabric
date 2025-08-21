// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  matrix.cppm
 *! \brief
 *!
 */

export module fabric.linalg:matrix;

import std;

import :vector;

export template <typename T, std::size_t ROWS, std::size_t COLUMNS>
struct mat {
  using scalar = T;
  template <std::size_t N>
  using vec = vec<scalar, N>;
  scalar data[ROWS * COLUMNS]{};

  mat() = default;

  mat(const mat& other)            = default;
  mat(mat&& other)                 = default;
  mat& operator=(const mat& other) = default;
  mat& operator=(mat&& other)      = default;

  explicit constexpr mat(std::array<std::array<T, COLUMNS>, ROWS> values) {
    for (std::size_t i = 0; i < ROWS; ++i) {
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        data[i * COLUMNS + j] = values[i][j];
      }
    }
  }

  constexpr mat(std::initializer_list<std::array<T, COLUMNS>> values) {
    std::size_t i = 0;
    for (const auto & row : values) {
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        data[i * COLUMNS + j] = row[j];
      }
      ++i;
    }
  }

  static constexpr mat zeros() {
    return mat{};
  }

  static constexpr mat identity() {
    mat m{};
    for (std::size_t i = 0; i < std::min(ROWS, COLUMNS); ++i) {
      m.data[i * COLUMNS + i] = 1;
    }
    return m;
  }

  vec<COLUMNS> get_row(std::size_t row) {
    vec<COLUMNS> ret{};
    for (std::size_t col = 0; col < COLUMNS; ++col) {
      ret[col] = (*this)[row, col];
    }
    return ret;
  }

  vec<ROWS> get_column(std::size_t col) {
    vec<ROWS> ret{};
    for (std::size_t row = 0; row < ROWS; ++row) {
      ret[row] = (*this)[row, col];
    }
    return ret;
  }

  vec<COLUMNS> operator[](std::size_t row) {
    return get_row(row);
  }

  T& operator[](std::size_t row, std::size_t column) {
    return data[row * COLUMNS + column];
  }

  const T& operator[](std::size_t row, std::size_t column) const {
    return data[row * COLUMNS + column];
  }

  mat& operator+=(const mat& other) {
    for (std::size_t i = 0; i < ROWS; ++i) {
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        data[i * COLUMNS + j] += other.data[i * COLUMNS + j];
      }
    }
    return *this;
  }

  mat operator+(const mat& other) const {
    mat ret{*this};
    ret += other;
    return ret;
  }

  mat& operator-=(const mat& other) {
    for (std::size_t i = 0; i < ROWS; ++i) {
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        data[i * COLUMNS + j] -= other.data[i * COLUMNS + j];
      }
    }
    return *this;
  }

  mat operator-(const mat& other) const {
    mat ret{*this};
    ret -= other;
    return ret;
  }

  mat& operator*=(scalar factor) {
    for (std::size_t i = 0; i < ROWS; ++i) {
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        data[i * COLUMNS + j] *= factor;
      }
    }
    return *this;
  }

  mat operator*(scalar factor) const {
    mat ret{*this};
    ret *= factor;
    return ret;
  }


  mat& operator/=(scalar factor) {
    for (std::size_t i = 0; i < ROWS; ++i) {
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        data[i * COLUMNS + j] /= factor;
      }
    }
    return *this;
  }

  mat operator/(scalar factor) const {
    mat ret{*this};
    ret /= factor;
    return ret;
  }

  vec<ROWS> operator*(const vec<COLUMNS>& v) const {
    vec<ROWS> ret{};
    for (std::size_t i = 0; i < ROWS; ++i) {
      scalar accum{0};
      for (std::size_t j = 0; j < COLUMNS; ++j) {
        accum += data[i * COLUMNS + j] * v[j];
      }
      ret[i] = accum;
    }
    return ret;
  }

  mat& operator*=(const mat& other) {
    mat result{};
    for (std::size_t i = 0; i < COLUMNS; ++i) {
      for (std::size_t j = 0; j < ROWS; ++j) {
        scalar accum{0};
        for (std::size_t k = 0; k < COLUMNS; ++k) {
          accum += (*this)[j,k] * other[k,i];
        }
        result[j,i] = accum;
      }
    }
    (*this) = result;
    return *this;
  }

  template <std::size_t N>
  mat<scalar, ROWS, N> operator*(const mat<scalar, COLUMNS, N>& other) const {
    mat<scalar, ROWS, N> ret{};
    for (std::size_t i = 0; i < N; ++i) {
      for (std::size_t j = 0; j < ROWS; ++j) {
        scalar accum{0};
        for (std::size_t k = 0; k < COLUMNS; ++k) {
          accum += (*this)[j,k] * other[k,i];
        }
        ret[j,i] = accum;
      }
    }
    return ret;
  }
};
