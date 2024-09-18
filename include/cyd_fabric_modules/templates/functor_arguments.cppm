// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.templates.functor_arguments;

export import fabric.templates.functor_concept;

namespace fabric {
  template<typename F, typename Ret, typename A, typename... Rest>
  A helper(Ret (F::* )(A, Rest...));

  template<typename F, typename Ret, typename A, typename... Rest>
  A helper(Ret (F::* )(A, Rest...) const);

  template<typename F, typename Ret, typename A>
  A helper(Ret (F::* )(A));

  template<typename F, typename Ret, typename A>
  A helper(Ret (F::* )(A) const);

  template<typename F, typename Ret>
  void helper(Ret (F::* )() const);

  export
  template<non_template_functor F>
  struct first_argument {
    typedef decltype( helper(&F::operator()) ) type;
  };

  export
  template<template<typename> typename F, typename FA>
    requires template_functor<F, FA>
  struct first_argument<F<FA>> {
    typedef decltype( helper(&F<FA>::operator()) ) type;
  };
}
