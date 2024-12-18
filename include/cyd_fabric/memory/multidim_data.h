// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_STD_MULTIDIM_DATA_H
#define CYD_UI_STD_MULTIDIM_DATA_H

#include <cstddef>
#include <array>
#include <cstdlib>
#include <cmath>
#include <cstring>

using size_t = std::size_t;
using u8     = unsigned char;

template<typename E, size_t D>
struct md_buffer_t {
  std::array<size_t, D> size;
  E* data = nullptr;

  explicit md_buffer_t(std::array<size_t, D> size): size(size) {
    size_t total_size = compute_total_size(this->size);
    data              = (E*)calloc(1, sizeof(E) * total_size);
  }

  explicit md_buffer_t(std::array<size_t, D> size, E*& data_): size(size) {
    data              = data_;
    data_ = nullptr;
  }

  template<std::size_t ... sizes>
  explicit md_buffer_t(): size {sizes...} {
    size_t total_size = compute_total_size(this->size);
    data              = (E*)calloc(1, sizeof(E) * total_size);
  }

  template<typename... S>
  explicit md_buffer_t(S... sizes): size {sizes...} {
    size_t total_size = compute_total_size(this->size);
    data              = (E*)calloc(1, sizeof(E) * total_size);
  }

  virtual ~md_buffer_t() {
    free(data);
  }

  md_buffer_t(const md_buffer_t &rhl) {
    this->size        = rhl.size;
    size_t total_size = compute_total_size(this->size);
    data              = (E*)calloc(1, sizeof(E) * total_size);
    memcpy(data, rhl.data, sizeof(E) * total_size);
  }

  md_buffer_t(md_buffer_t &&rhl) {
    this->size = rhl.size;
    data       = rhl.data;
    for (auto &item: rhl.size) {
      item = 0;
    }
    rhl.data = nullptr;
  }

  md_buffer_t &operator=(const md_buffer_t &rhl) {
    auto copy = md_buffer_t {rhl};
    this->swap(copy);
    return *this;
  }

  md_buffer_t &operator=(md_buffer_t &&rhl) {
    auto copy = md_buffer_t {std::forward<md_buffer_t &&>(rhl)};
    this->swap(copy);
    return *this;
  }

  void swap(md_buffer_t &other) {
    auto tmp_size = other.size;
    auto tmp_data = other.data;
    other.size    = this->size;
    other.data    = this->data;
    this->size    = tmp_size;
    this->data    = tmp_data;
  }

  void resize(std::array<size_t, D> new_size) {
    bool same = true;
    for (size_t i = 0; i < D; ++i) {
      if (size[i] != new_size[i]) {
        same = false;
        break;
      }
    }
    if (same) return;

    size_t total_size     = compute_total_size(this->size);
    size_t total_new_size = compute_total_size(new_size);

    //auto* old_data = this->data;
    this->data = (E*)realloc(this->data, sizeof(E) * total_new_size);
    //this->data = (E*) calloc(1, sizeof(E) * total_new_size);
    //this->copy_from(old_data, {0, 0}, {0, 0}, total_size > total_new_size ? new_size : size);
    //free(old_data);
    size = new_size;
  }

  E* get(std::array<size_t, D> indices) {
    size_t index = compute_index(indices);
    return &data[index];
  }

  void set(std::array<size_t, D> indices, const E &value) {
    size_t index = compute_index(indices);
    if (index < compute_total_size(size)) {
      data[index] = value;
    }
  }

  template<typename... C>
  E &operator[](C... indices) {
    return *get({indices...});
  }

  template<typename... C>
  const E &operator[](C... indices) const {
    size_t index = compute_index({indices...});
    return data[index];
  }

private:
  size_t compute_total_size(std::array<size_t, D> s) const {
    size_t total_size = 1;
    for (size_t i = 0; i < D; ++i) {
      total_size *= s[i];
    }
    return total_size;
  }

  size_t compute_index(std::array<size_t, D> coords) const {
    // i = x + X*y + X*Y*z + ...
    auto iter         = coords.begin();
    size_t index      = *iter;
    size_t multiplier = size[0];
    iter              = std::next(iter);
    for (size_t i = 1; i < D; ++i) {
      index += multiplier * *iter;
      multiplier *= size[i];
      iter = std::next(iter);
    }
    return index;
  }

  void copy_from(
    const E* source,
    std::array<size_t, D> src_coords,
    std::array<size_t, D> dst_coords,
    std::array<size_t, D> _size
  ) {
    size_t stride               = _size[0];
    size_t total_strides        = compute_total_size(_size) / stride;
    std::array<size_t, D> d_pos = dst_coords;
    std::array<size_t, D> s_pos = src_coords;

    size_t d_abs_index;
    size_t s_abs_index;
    for (size_t i = 0; i < total_strides; ++i) {
      d_abs_index = compute_index(d_pos);
      s_abs_index = compute_index(s_pos);
      memcpy(&this->data[d_abs_index], &source[s_abs_index], sizeof(E) * stride);

      for (size_t d = 1; d < D; ++d) {
        if (0 == (i + 1) % _size[d]) {
          d_pos[d]++;
          s_pos[d]++;
        }
      }
    }
  }
};

#endif //CYD_UI_STD_MULTIDIM_DATA_H
