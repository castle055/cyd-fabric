// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  rule_operators.cppm
 *! \brief 
 *!
 */

export module fabric.grammar.rules;

import :expressions;

export
{
//* ALTERNATION OPERATOR
template <char C1, char C2>
consteval auto operator|(fabric::terminal<C1>, fabric::terminal<C2>) -> fabric::alternation_expr<fabric::terminal<C1>, fabric::terminal<C2>> {
  return {};
}

template <char C, typename E>
consteval auto operator|(fabric::terminal<C>, E) -> fabric::alternation_expr<fabric::terminal<C>, E> {
  return {};
}

template <char C, typename E>
consteval auto operator|(E, fabric::terminal<C>) -> fabric::alternation_expr<E, fabric::terminal<C>> {
  return {};
}

template <typename E1, typename E2>
consteval auto operator|(E1, E2) -> fabric::alternation_expr<E1, E2> {
  return {};
}

template <char C, typename ...Exprs>
consteval auto operator|(fabric::alternation_expr<Exprs...>, fabric::terminal<C>) -> fabric::alternation_expr<Exprs..., fabric::terminal<C>> {
  return {};
}

template <char C, typename ...Exprs>
consteval auto operator|(fabric::terminal<C>, fabric::alternation_expr<Exprs...>) -> fabric::alternation_expr<fabric::terminal<C>, Exprs...> {
  return {};
}

template <typename E, typename ...Exprs>
consteval auto operator|(fabric::alternation_expr<Exprs...>, E) -> fabric::alternation_expr<Exprs..., E> {
  return {};
}

template <typename E, typename ...Exprs>
consteval auto operator|(E, fabric::alternation_expr<Exprs...>) -> fabric::alternation_expr<E, Exprs...> {
  return {};
}

//* SEQUENCE OPERATOR
template <char C1, char C2>
consteval auto operator,(fabric::terminal<C1>, fabric::terminal<C2>) -> fabric::sequence_expr<fabric::terminal<C1>, fabric::terminal<C2>> {
  return {};
}

template <char C, typename E>
consteval auto operator,(fabric::terminal<C>, E) -> fabric::sequence_expr<fabric::terminal<C>, E> {
  return {};
}

template <char C, typename E>
consteval auto operator,(E, fabric::terminal<C>) -> fabric::sequence_expr<E, fabric::terminal<C>> {
  return {};
}

template <typename E1, typename E2>
consteval auto operator,(E1, E2) -> fabric::sequence_expr<E1, E2> {
  return {};
}

template <char C, typename ...Exprs>
consteval auto operator,(fabric::sequence_expr<Exprs...>, fabric::terminal<C>) -> fabric::sequence_expr<Exprs..., fabric::terminal<C>> {
  return {};
}

template <char C, typename ...Exprs>
consteval auto operator,(fabric::terminal<C>, fabric::sequence_expr<Exprs...>) -> fabric::sequence_expr<fabric::terminal<C>, Exprs...> {
  return {};
}

template <typename E, typename ...Exprs>
consteval auto operator,(fabric::sequence_expr<Exprs...>, E) -> fabric::sequence_expr<Exprs..., E> {
  return {};
}

template <typename E, typename ...Exprs>
consteval auto operator,(E, fabric::sequence_expr<Exprs...>) -> fabric::sequence_expr<E, Exprs...> {
  return {};
}

//* OTHER OPERATORS
template <char... Chars>
consteval auto operator*(fabric::charset<false, false, Chars...> e) -> fabric::charset<true, true, Chars...> {
  return {};
}

template <char... Chars>
consteval auto operator+(fabric::charset<false, false, Chars...> e) -> fabric::charset<true, false, Chars...> {
  return {};
}

template <typename E>
consteval auto operator*(E e) -> fabric::repetition_expr<E, true> {
  return {};
}

template <typename E>
consteval auto operator+(E e) -> fabric::repetition_expr<E, false> {
  return {};
}

template <typename E>
consteval auto operator~(E e) -> fabric::optional_expr<E> {
  return {};
}

template <typename E>
consteval auto operator!(E e) -> fabric::skip_expr<E> {
  return {};
}


// template <template <typename ...> typename E, typename ...Exprs>
// consteval auto operator!(E<Exprs...> e) -> E<Exprs...> {
//   return {};
// }
//
// template <template <typename ...> typename E, typename ...Exprs>
// consteval auto operator~(E<Exprs...> e) -> E<Exprs...> {
//   return {};
// }
//
// template <template <typename ...> typename E, typename ...Exprs>
// consteval auto operator&(E<Exprs...> e) -> E<Exprs...> {
//   return {};
// }
//
// template <template <typename ...> typename E, typename ...Exprs>
// consteval auto operator-(E<Exprs...> e) -> E<Exprs...> {
//   return {};
// }
}
