/*! \file  rule_expressions.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:rule_expressions;

import std;

export namespace fabric {
 template <char C>
 struct terminal {
  static constexpr char name[2] {C, '\0'};
  static constexpr char value = C;
 };

 template <bool repeat, bool allow_none, char ...Chars>
 struct charset {
  static constexpr char name[8 + sizeof...(Chars) + 2] {'c', 'h', 'a', 'r', 's', 'e', 't', '(', Chars..., ')'};
  static constexpr char chars[sizeof...(Chars) + 1] {Chars...};
  static constexpr std::size_t count = sizeof...(Chars);
 };

 template <char ...Chars>
 auto to_charset(terminal<Chars> ...chars) -> charset<false, false, Chars...> {
  return {};
 }



 template <typename Rule>
 struct recurse { };

 template <typename ...Expr>
 struct rule_expr { };

 template <typename Expr>
 struct optional_expr: rule_expr<Expr> { };

 template <typename Expr>
 struct skip_expr: rule_expr<Expr> { };

 template <typename Expr, bool allow_none>
 struct repetition_expr: rule_expr<Expr> { };

 template <typename ...Exprs>
 struct alternation_expr: rule_expr<Exprs...> { };

 template <typename ...Exprs>
 struct sequence_expr: rule_expr<Exprs...> { };
}
