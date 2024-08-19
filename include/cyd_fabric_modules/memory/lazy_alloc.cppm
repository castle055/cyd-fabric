// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.memory.lazy_alloc;

import std;

export template<typename T, typename ...Args>
struct lazy_alloc {
  explicit lazy_alloc(Args... args) {
    init = [args...](lazy_alloc<T>* self) {
      self->val = std::make_shared<T>(args...);
    };
  }
  ~lazy_alloc() = default;
  
  T* operator->() {
    if (nullptr == val) {
      init(this);
    }
    return val.get();
  }
  
  operator T*() {
    if (nullptr == val) {
      init(this);
    }
    return val.get();
  }
  
  std::function<void(lazy_alloc<T>*)> init = [](lazy_alloc<T>* self) {
    self->val = std::make_shared<T>();
  };
  std::shared_ptr<T> val = nullptr;
};
