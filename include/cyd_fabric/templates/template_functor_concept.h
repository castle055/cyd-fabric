//
// Created by castle on 4/16/24.
//

#ifndef TEMPLATE_FUNCTOR_CONCEPT_H
#define TEMPLATE_FUNCTOR_CONCEPT_H

namespace cyd::fabric {
  template<typename F>
  concept non_template_functor = requires
  {
    &F::operator();
  };
  template<template<typename> typename F, typename T>
  concept template_functor = requires
  {
    &F<T>::operator();
  };
}

#endif //TEMPLATE_FUNCTOR_CONCEPT_H
