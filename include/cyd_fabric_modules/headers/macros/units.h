// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FABRIC_UNITS_MACROS_H
#define FABRIC_UNITS_MACROS_H

#define UNIT_SYMBOL(...) static constexpr inline std::string symbol() noexcept { return (__VA_ARGS__); }

#define SCALE(NAME)       \
namespace NAME {          \
  struct scale {          \
    UNIT_SYMBOL(#NAME)    \
  };                      \
  template <typename Q> concept quantity = fabric::units::Quantity<Q, scale>; \
}                         \
namespace NAME

#define DERIVED_SCALE(NAME, ...) \
namespace NAME {         \
  using scale = typename reduce_scale<__VA_ARGS__>::type;       \
  template <typename Q> concept quantity = fabric::units::Quantity<Q, scale>; \
}                        \
namespace NAME

#define UNIT(NAME, SYMBOL, FACTOR_NUM, FACTOR_DEN) \
struct NAME {                                      \
  using scale = scale;                             \
  template <typename T>                            \
  using factor = fabric::ratio<T, FACTOR_NUM, FACTOR_DEN>; \
  using reduce = NAME;                             \
  UNIT_SYMBOL(SYMBOL)                              \
};

#define UNIT_IN_SCALE(SCALE, NAME, SYMBOL, FACTOR_NUM, FACTOR_DEN) \
struct NAME {                                                      \
  using scale = SCALE;                                             \
  template <typename T>                                            \
  using factor = fabric::ratio<T, FACTOR_NUM, FACTOR_DEN>;         \
  using reduce = NAME;                                             \
  UNIT_SYMBOL(SYMBOL)                                              \
};

#define METRIC_SCALE(UNIT_NAME, SYMBOL, FACTOR_NUM, FACTOR_DEN) \
    UNIT(nano ##UNIT_NAME, "n" SYMBOL, 1UL*(FACTOR_NUM)        , 1000000000UL*(FACTOR_DEN)) \
    UNIT(micro##UNIT_NAME, "u" SYMBOL, 1UL*(FACTOR_NUM)        , 1000000UL*(FACTOR_DEN)) \
    UNIT(milli##UNIT_NAME, "m" SYMBOL, 1UL*(FACTOR_NUM)        , 1000UL*(FACTOR_DEN)) \
    UNIT(centi##UNIT_NAME, "c" SYMBOL, 1UL*(FACTOR_NUM)        , 100UL*(FACTOR_DEN)) \
    UNIT(deci ##UNIT_NAME, "d" SYMBOL, 1UL*(FACTOR_NUM)        , 10UL*(FACTOR_DEN)) \
    UNIT(UNIT_NAME       ,     SYMBOL, 1UL*(FACTOR_NUM)        , 1UL*(FACTOR_DEN)) \
    UNIT(deca ##UNIT_NAME, "D" SYMBOL, 10UL*(FACTOR_NUM)       , 1UL*(FACTOR_DEN)) \
    UNIT(hecto##UNIT_NAME, "h" SYMBOL, 100UL*(FACTOR_NUM)      , 1UL*(FACTOR_DEN)) \
    UNIT(kilo ##UNIT_NAME, "k" SYMBOL, 1000UL*(FACTOR_NUM)     , 1UL*(FACTOR_DEN)) \
    UNIT(mega ##UNIT_NAME, "M" SYMBOL, 1000000UL*(FACTOR_NUM)  , 1UL*(FACTOR_DEN))


#define SCALE_CONVERSION(FROM, TO) \
template <> struct fabric::units::scale_conversion_t<FROM, TO> { \
  using from_scale = FROM; \
  using to_scale = TO;

#define SCALE_FORWARD_CONVERSION(...) \
  template <typename U_FROM, typename U_TO, typename T> \
    requires fabric::units::CompareScales<from_scale, typename U_FROM::scale> \
             && fabric::units::CompareScales<to_scale, typename U_TO::scale> \
  static quantity_t<U_TO, T> forward(const quantity_t<U_FROM, T>& quantity) { \
    quantity_t<U_TO, T> result { \
      quantity.value * U_FROM::template factor<T>::numerator / U_FROM::template factor<T>::denominator \
    }; \
    auto it = result.value; \
    result.value = __VA_ARGS__; \
    result.value = result.value * U_TO::template factor<T>::denominator / U_TO::template factor<T>::numerator; \
    return result; }

#define SCALE_BACKWARD_CONVERSION(...) \
  template <typename U_FROM, typename U_TO, typename T> \
    requires fabric::units::CompareScales<to_scale, typename U_FROM::scale> \
             && fabric::units::CompareScales<from_scale, typename U_TO::scale> \
  static quantity_t<U_TO, T> backward(const quantity_t<U_FROM, T>& quantity) { \
    quantity_t<U_TO, T> result { \
      quantity.value * U_FROM::template factor<T>::numerator / U_FROM::template factor<T>::denominator \
    }; \
    auto it = result.value; \
    result.value = __VA_ARGS__; \
    result.value = result.value * U_TO::template factor<T>::denominator / U_TO::template factor<T>::numerator; \
    return result; \
  } };


#endif //FABRIC_UNITS_MACROS_H
