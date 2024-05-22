// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/22/24.
//

#ifndef FABRIC_UNITS_MACROS_H
#define FABRIC_UNITS_MACROS_H

#define SCALE(NAME, ...) \
namespace NAME {         \
  struct scale {};       \
  template <typename Q> concept quantity = Quantity<Q, scale>; \
}                        \
namespace NAME __VA_ARGS__

#define UNIT_SYMBOL(...) static constexpr inline std::string symbol() noexcept { return (__VA_ARGS__); }

#define UNIT(NAME, SYMBOL, FACTOR_NUM, FACTOR_DEN) \
struct NAME {                                      \
  using scale = scale;                             \
  template <typename T>                            \
  using factor = cyd::fabric::ratio<T, FACTOR_NUM, FACTOR_DEN>; \
  using reduce = NAME;                             \
  UNIT_SYMBOL(SYMBOL)                              \
};

#endif //FABRIC_UNITS_MACROS_H
