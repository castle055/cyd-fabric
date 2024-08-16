// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  vector.cppm
 *! \brief 
 *!
 */

export module fabric.linalg:vector;

import std;
import fabric.ts.packs;

export template<typename T, std::size_t SIZE>
struct vec {
  std::array<T, SIZE> data { };
  using scalar = T;

  vec(std::initializer_list<T> values) {
    std::size_t i = 0;
    for (const auto &item: values) {
      (*this)[i] = item;
      ++i;
    }
  }

  scalar &operator[](std::size_t index) {
    throw_if_out_of_bounds(index);
    return this->data[index];
  }

  const scalar &operator[](std::size_t index) const {
    throw_if_out_of_bounds(index);
    return this->data[index];
  }

  bool operator==(const vec &rhl) const {
    for (std::size_t i = 0; i < SIZE; ++i) {
      if ((*this)[i] != rhl[i]) return false;
    }
    return true;
  }

  template<typename S, typename O = decltype(std::declval<scalar>() * std::declval<S>())>
  vec<O, SIZE> operator*(S scalar1) const {
    vec<O, SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = (*this)[i] * scalar1;
    }
    return ret;
  }

  template<typename S, typename O = decltype(std::declval<scalar>() / std::declval<S>())>
  vec<O, SIZE> operator/(S scalar1) const {
    vec<O, SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = (*this)[i] / scalar1;
    }
    return ret;
  }


  template<typename V, typename O = decltype(std::declval<scalar>() + std::declval<typename V::scalar>())>
  vec<O, SIZE> operator+(const V &other) const & {
    vec<O, SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = (*this)[i] + other[i];
    }
    return ret;
  }

  template<typename V, typename O = decltype(std::declval<scalar>() + std::declval<typename V::scalar>())>
  vec<O, SIZE> operator+(const V &other) const && {
    vec<O, SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = operator[](i) + other[i];
    }
    return ret;
  }

  template<typename V, typename O = decltype(std::declval<scalar>() - std::declval<typename V::scalar>())>
  vec<O, SIZE> operator-(const V &other) const & {
    vec<O, SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = (*this)[i] - other[i];
    }
    return ret;
  }

  template<typename V, typename O = decltype(std::declval<scalar>() - std::declval<typename V::scalar>())>
  vec<O, SIZE> operator-(const V &other) const && {
    vec<O, SIZE> ret { };
    for (std::size_t i = 0; i < SIZE; ++i) {
      ret[i] = operator[](i) - other[i];
    }
    return ret;
  }

  decltype(std::declval<scalar>() * std::declval<scalar>()) mag2() const {
    decltype(std::declval<scalar>() * std::declval<scalar>()) m2 = 0;
    for (std::size_t i = 0; i < SIZE; ++i) {
      m2 = m2 + ((*this)[i] * (*this)[i]);
    }
    return m2;
  }

  std::size_t size() const {
    return SIZE;
  }

  [[nodiscard]] std::string to_string() const {
    std::stringstream sts { };
    sts << "(";
    for (std::size_t i = 0; i < SIZE; ++i) {
      if constexpr (requires { (*this)[i].to_string(); }) {
        sts << (*this)[i].to_string();
        if (i < SIZE - 1) {
          sts << ", ";
        }
      }
    }
    sts << ")";
    return sts.str();
    // return "";
  }

private:
  inline void throw_if_out_of_bounds(std::size_t index) const {
    if (index >= SIZE) {
      throw std::runtime_error {std::format("[OUT_OF_RANGE] {} not in range [0, {})", index, SIZE)};
    }
  }
};

export template<typename... V>
vec(V...) -> vec<cyd::fabric::ts::packs::get_first<V...>, sizeof...(V)>;

