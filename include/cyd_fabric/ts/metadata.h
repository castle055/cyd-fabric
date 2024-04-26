//
// Created by castle on 4/27/24.
//

#ifndef METADATA_H
#define METADATA_H

#include "apply.h"

namespace cyd::fabric::ts {
  template<typename T>
  struct metadata;

  template<typename T>
  struct metadata<T>: with_type<T> {
  };

  template<template <typename...> typename T, typename... Args>
  struct metadata<T<Args...>>: with_type<T<Args...>> {
  };
}

#endif //METADATA_H
