// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.templates.functor_concept;

export
namespace fabric {
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
