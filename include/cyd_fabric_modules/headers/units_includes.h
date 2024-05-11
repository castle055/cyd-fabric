//
// Created by castle on 5/11/24.
//

#ifndef UNITS_INCLUDES_H
#define UNITS_INCLUDES_H

#include "units_preface.h"

#define SCALE(NAME, ...) \
namespace NAME {         \
  struct scale {};       \
  template <typename Q> concept quantity = Quantity<Q, scale>; \
}                        \
namespace NAME __VA_ARGS__
#define UNIT(NAME, SYMBOL, FACTOR_NUM, FACTOR_DEN) \
struct NAME {                                      \
 _Pragma("GCC diagnostic push")                    \
 _Pragma("GCC diagnostic ignored \"-Wchanges-meaning\"")\
  using scale = scale;                             \
 _Pragma("GCC diagnostic pop")                     \
  template <typename T>                            \
  using factor = cyd::fabric::ratio<T, FACTOR_NUM, FACTOR_DEN>; \
  using reduce = NAME;                             \
  UNIT_SYMBOL(SYMBOL)                              \
};

#endif //UNITS_INCLUDES_H
