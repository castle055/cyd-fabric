// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  expressions.cppm
 *! \brief 
 *!
 */

export module expressions;
import std;

// addition
// negative -> (-1)*( )
// multiplication - division
// exponential - logarithm
// derivation - integrationo

template<typename... Cs>
struct expr_node {
};

template<typename N>
concept DerivesExprNode = std::derived_from<N, typename N::expr_node>;

template<typename S>
struct symbol: expr_node<S> {
};

template<DerivesExprNode... Cs>
struct addition: expr_node<Cs...> {
};

template<DerivesExprNode... Cs>
struct multiplication: expr_node<Cs...> {
};

template<DerivesExprNode N, DerivesExprNode D>
struct division: expr_node<N, D> {
};

template<DerivesExprNode Left, DerivesExprNode Right>
struct equality {
};

namespace newton {
  struct second_law {
    struct sym_F: symbol<sym_F> {
    };

    struct sym_m: symbol<sym_m> {
    };

    struct sym_a: symbol<sym_a> {
    };

    using eq = equality<sym_F, multiplication<sym_m, sym_a>>;

    template<typename Symbol>
    struct isolate;
  };

  template<>
  struct second_law::isolate<second_law::sym_a>
    : equality<sym_a, division<sym_F, sym_m>> {
  };
}

void testest() {
  newton::second_law::isolate<newton::second_law::sym_a> e;
  e;
}

// decltype(it) identity(auto it) {
  // return it;
// }

auto identity(auto it) -> decltype(it) {
  return it;
}
