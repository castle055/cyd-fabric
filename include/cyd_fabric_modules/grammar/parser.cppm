// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  parser.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:parser;

import std;

import :concepts;
import :ast;
import :rule_expressions;

export import :parser_input_string;
export import :parser_input_fs_path;

export namespace fabric {
  struct parser_log_t: std::vector<std::pair<std::size_t, std::string>> {
    void dump(auto& out) const {
      for (const auto & [line, msg]: (*this)) {
        out << "(" << line << ") " << msg << std::endl;
      }
    }

    int progress = 0;
  };

  struct parser_config_t {
    bool dont_skip = false;
  };

  template<typename Rule>
  struct parser {};

  template<RuleConcept Rule>
  struct parser<Rule> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      auto restore_point = in.create_restore_point();

      if (in.is_at_end()) {
        return false;
      }

      if (Rule::transient) {
        if (parser<typename Rule::rule>::operator()(in, node, parser_log, config)) {
          return true;
        } else {
          in.restore(restore_point);
          return false;
        }
      } else {
        auto child_node  = std::make_shared<Rule>();
        child_node->text = Rule::name;
        if (parser<typename Rule::rule>::operator()(in, child_node, parser_log, config)) {
          if constexpr (requires { Rule::action(std::declval<typename Rule::sptr>()); }) {
            Rule::action(child_node);
          }
          node->children.push_back(child_node);
          // std::cout << std::format("    Result of parse rule at ({}): {} ", in.get_current_offset(), Rule::name);
          int progress = (100 * in.get_current_offset()) / in.get_size();
          if (progress != parser_log.progress) {
            // TODO - Make this available as an option
            // std::cout << "[" << std::to_string(progress) << " %]" << std::endl;
            parser_log.progress = progress;
          }
          return true;
        } else {
          in.restore(restore_point);
          if (parser_log.empty()) {
            parser_log.emplace_back(in.get_current_offset(),
                                    std::format("Expected '{}' at {}", std::string(Rule::name), in.get_current_offset()));
          }

          return false;
        }
      }
    }
  };

  template <typename ...Exprs>
  struct parser<sequence_expr<Exprs...>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      const auto restore_point = in.create_restore_point();

      if ((parser<Exprs>::operator()(in, node, parser_log, config) && ...)) {
        return true;
      } else {
        in.restore(restore_point);
        return false;
      }
    }
  };

  template <typename ...Exprs>
  struct parser<alternation_expr<Exprs...>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      const auto restore_point = in.create_restore_point();

      return (([&]<typename Expr> -> bool {
        const bool res = parser<Expr>::operator()(in, node, parser_log, config);
        if (res) {
          parser_log.clear();
        } else {
          in.restore(restore_point);
          if constexpr (RuleConcept<Expr>) {
            if (!Expr::transient) {
              parser_log.emplace_back(in.get_current_offset(),
                                      std::format("alternative '{}' at {}", std::string(Expr::name), in.get_current_offset()));
            }
          }
        }
        return res;
      }.template operator()<Exprs>()) || ... );
    }
  };

  template <typename Expr>
  struct parser<optional_expr<Expr>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      const auto restore_point = in.create_restore_point();

      if (not parser<Expr>::operator()(in, node, parser_log, config)) {
        in.restore(restore_point);
      }
      parser_log.clear();
      return true;
    }
  };

  template <typename Expr>
  struct parser<skip_expr<Expr>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      const auto restore_point = in.create_restore_point();

      auto skip_node = config.dont_skip ? node : std::make_shared<Node>();
      if (not parser<Expr>::operator()(in, skip_node, parser_log, config)) {
        in.restore(restore_point);
        return false;
      }
      parser_log.clear();
      return true;
    }
  };

  template <typename Expr, bool allow_none>
  struct parser<repetition_expr<Expr, allow_none>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      const auto restore_point = in.create_restore_point();

      unsigned int count = 0;
      auto prev = restore_point;
      auto temp_node = std::make_shared<Node>();
      while (parser<Expr>::operator()(in, temp_node, parser_log, config)) {
        ++count;
        prev = in.create_restore_point();
        bool appending_text = true;
        for (const auto & child : temp_node->children) {
          if (appending_text && child->is_text && !node->children.empty() && node->children.back()->is_text) {
            node->children.back()->text.append(child->text);
          } else {
            appending_text = false;
            node->children.push_back(child);
          }
        }
        temp_node->children.clear();
        parser_log.clear();
      }

      if (count == 0) {
        in.restore(restore_point);
        return allow_none;
      } else {
        in.restore(prev);
        return true;
      }
    }
  };

  template <typename Expr>
  struct parser<recurse<Expr>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      return parser<Expr>::operator()(in, node, parser_log, config);
    }
  };

  template <char C>
  struct parser<terminal<C>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      if (!in.is_at_end() && C == in.get_current_char()) {
        if (node->children.empty()) {
          auto text_child     = std::make_shared<node_t>();
          text_child->is_text = true;
          text_child->text.push_back(in.get_current_char());
          node->children.push_back(text_child);
        } else {
          auto possible_text_child = node->children[node->children.size() - 1];
          if (possible_text_child->is_text) {
            possible_text_child->text.push_back(in.get_current_char());
          } else {
            auto text_child     = std::make_shared<node_t>();
            text_child->is_text = true;
            text_child->text.push_back(in.get_current_char());
            node->children.push_back(text_child);
          }
        }
        in.seek(1);
        return true;
      } else {
        return false;
      }
    }
  };

  template <bool repeat, bool allow_none, char... Chars>
  struct parser<charset<repeat, allow_none, Chars...>> {
    template<typename Input, RuleConcept Node>
    static inline bool operator()(parser_input_t<Input>& in, std::shared_ptr<Node> node, parser_log_t& parser_log, const parser_config_t& config) {
      if (in.is_at_end()) {
        return false;
      }
      char current_char = in.get_current_char();
      if (((current_char == Chars) || ...)) {
        if (node->children.empty()) {
          auto text_child     = std::make_shared<node_t>();
          text_child->is_text = true;
          text_child->text.push_back(current_char);
          node->children.push_back(text_child);
        } else {
          auto possible_text_child = node->children[node->children.size() - 1];
          if (possible_text_child->is_text) {
            possible_text_child->text.push_back(current_char);
          } else {
            auto text_child     = std::make_shared<node_t>();
            text_child->is_text = true;
            text_child->text.push_back(current_char);
            node->children.push_back(text_child);
          }
        }
        in.seek(1);

        if (repeat) {
          auto start = in.create_restore_point();
          std::size_t count = 0UL;
          current_char = in.get_current_char();
          while (!in.is_at_end() && ((current_char == Chars) || ...)) {
            in.seek(1);
            current_char = in.get_current_char();
            ++count;
          }
          // Since we already appended one character we can assume this children to be a valid text node
          auto possible_text_child = node->children[node->children.size() - 1];
          in.restore(start);
          in.append_to(possible_text_child->text, count);
        }

        return true;
      } else {
        return repeat && allow_none;
      }
    }
  };



  template <GrammarConcept Grammar>
  struct parse_result {
    const bool ok;
    const typename Grammar::start::sptr ast;
    const parser_log_t log;
  };

  template <GrammarConcept Grammar, typename Input>
  parse_result<Grammar> parse(const Input& input_, parser_config_t config = {}) {
    parser_input_t<Input> input{input_};
    auto node = std::make_shared<typename Grammar::start>();
    parser_log_t parser_log{};

    if (parser<typename Grammar::start>::operator()(input, node, parser_log, config)) {
      return {parser_log.empty(), std::dynamic_pointer_cast<typename Grammar::start>(node->children[0]), parser_log};
    } else {
      return {false, nullptr, parser_log};
    }
  }
}