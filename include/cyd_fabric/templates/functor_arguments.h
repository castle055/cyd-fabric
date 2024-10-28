// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FUNCTOR_ARGUMENTS_H
#define FUNCTOR_ARGUMENTS_H

#include "template_functor_concept.h"

namespace cyd::fabric {
  template<typename F, typename Ret, typename A, typename... Rest>
  A helper(Ret (F::* )(A, Rest...));

  template<typename F, typename Ret, typename A, typename... Rest>
  A helper(Ret (F::* )(A, Rest...) const);

  template<typename F, typename Ret>
  void helper(Ret (F::* )() const);


  template<non_template_functor F>
  struct first_argument {
    typedef decltype( helper(&F::operator()) ) type;
  };

  template<template<typename> typename F, typename FA>
    requires template_functor<F, FA>
  struct first_argument<F<FA>> {
    typedef decltype( helper(&F<FA>::operator()) ) type;
  };
}

#endif //FUNCTOR_ARGUMENTS_H
