/*! \file  concepts.cppm
 *! \brief 
 *!
 */

export module fabric.grammar:concepts;

import std;

import :ast;

export namespace fabric {
  template <typename R>
  concept RuleConcept = requires
  {
    requires std::derived_from<R, node_t>;
    typename R::sptr;
    requires std::same_as<typename R::sptr, std::shared_ptr<R>>;
    typename R::rule;
    R::name;
    R::transient;
    requires std::same_as<decltype(R::transient), const bool>;
    // requires std::is_same_v<decltype(R::name), const char*>;
  };

  template <typename T>
  concept TerminalConcept = requires
  {
    T::name;
  };

  template <typename R>
  concept TransientRuleConcept = requires
  {
    requires RuleConcept<R>;
    requires R::transient == true;
  };

  template <typename G>
  concept GrammarConcept = requires
  {
    requires RuleConcept<typename G::start>;
    requires (!TransientRuleConcept<typename G::start>);
    G::name;
  };

}
