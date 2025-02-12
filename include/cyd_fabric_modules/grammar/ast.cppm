// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ast.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:ast;

import std;
export import reflect;

export namespace fabric {
  struct node_t {
    explicit node_t(refl::type_id_t type_id_): type_id(type_id_) {}

    virtual ~node_t() = default;

    using sptr = std::shared_ptr<node_t>;

    template <typename T>
    bool is_type() const {
      return type_id == refl::type_id<T>;
    }

    bool is_type(refl::type_id_t type_id_) const {
      return type_id == type_id_;
    }

    template <typename T>
    T* as() {
      return is_type<T>()? dynamic_cast<T*>(this): nullptr;
    }

    template <typename T>
    const T* as() const {
      return is_type<T>()? dynamic_cast<const T*>(this): nullptr;
    }

    const refl::type_id_t type_id;
    bool is_text = false;
    std::string text{};
    std::vector<sptr> children{};
  };
}
