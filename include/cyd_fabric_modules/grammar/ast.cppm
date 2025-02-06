/*! \file  ast.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:ast;

import std;

export namespace fabric {
  struct node_t {
    virtual ~node_t() = default;

    using sptr = std::shared_ptr<node_t>;

    bool is_text = false;
    std::string text{};
    std::vector<sptr> children{};
  };
}
