// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  circular_buffer.cppm
 *! \brief 
 *!
 */

export module fabric.memory.circular_buffer;

import std;

export namespace fabric {
 namespace overflow_strategy {
  struct overwrite {
   template<typename T>
   static constexpr bool same_as = std::same_as<T, overwrite>;
  };

  struct dont_write {
   template<typename T>
   static constexpr bool same_as = std::same_as<T, dont_write>;
  };

  template<typename E = std::runtime_error>
  struct throw_exception {
   template<typename T>
   static constexpr bool is_valid = requires
   {
    ([]<typename EE>(const throw_exception<EE> &e) -> void {
     throw EE("");
    })(T { });
   };

   template<typename T>
   static constexpr bool same_as = false;
   template<typename EE>
   static constexpr bool same_as<throw_exception<EE>> = std::same_as<E, EE>;
  };
 }

 template<typename T, std::size_t SIZE, typename OverflowStrategy = overflow_strategy::overwrite>
  requires overflow_strategy::overwrite::same_as<OverflowStrategy>
           || overflow_strategy::dont_write::same_as<OverflowStrategy>
           || overflow_strategy::throw_exception<>::is_valid<OverflowStrategy>
 class circular_buffer {
 public:
  void push_back(const T &t) {
   if (write_head_ + 1 % SIZE >= read_head_) {
    if constexpr (std::same_as<OverflowStrategy, overflow_strategy::overwrite>) {
     pop_front();
    } else if constexpr (std::same_as<OverflowStrategy, overflow_strategy::dont_write>) {
     return;
    } else {
     ([]<typename E>(const overflow_strategy::throw_exception<E> &e) -> void {
      throw E("Circular buffer is full.");
     })(OverflowStrategy { });
    }
   }

   data_[write_head_++] = t;
   wrap_write_head();
  }

  void pop_front() {
   if (empty()) return;
   ++read_head_;
   wrap_read_head();
  }

  void pop_back() {
   if (empty()) return;
   if (write_head_ == 0) {
    write_head_ = SIZE;
   } else {
    --write_head_;
   }
  }

  T &front() {
   return data_[read_head_];
  }

  T &back() {
   std::size_t back_ = write_head_ == 0 ? SIZE - 1 : write_head_ - 1;
   return data_[back_];
  }

  bool empty() const {
   return read_head_ == write_head_;
  }

  std::size_t size() const {
   if (empty()) return 0;
   if (read_head_ > write_head_) {
    return write_head_ + (SIZE - read_head_);
   } else {
    return write_head_ - read_head_;
   }
  }

  T &at(const std::size_t index) {
   return data_[compute_index(index)];
  }

  T &at(const std::size_t index) const {
   return data_[compute_index(index)];
  }

  T &operator[](const std::size_t index) {
   return data_[compute_index(index)];
  }

  T &operator[](const std::size_t index) const {
   return data_[compute_index(index)];
  }

 private:
  void wrap_read_head() {
   if (read_head_ >= SIZE) {
    read_head_ = read_head_ % SIZE;
   }
  }

  void wrap_write_head() {
   if (write_head_ >= SIZE) {
    write_head_ = write_head_ % SIZE;
   }
  }

  std::size_t compute_index(const std::size_t index) const {
   const std::size_t actual_size = size();
   if (actual_size == 0) {
    return 0;
   } else {
    return (read_head_ + (index % actual_size)) % SIZE;
   }
  }

 private:
  std::array<T, SIZE> data_ { };

  std::size_t read_head_ = 0, write_head_ = 0;
 };
}
