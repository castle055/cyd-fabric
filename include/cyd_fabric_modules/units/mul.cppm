//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/units_preface.h>
export module fabric.units:mul;
export import fabric.templates.ratio;

namespace cyd::fabric::units {
  export template<typename Product1, typename... Products>
  struct mul {
    using scale  = mul<typename Product1::scale, typename Products::scale...>;
    using reduce = mul<typename Product1::reduce, typename Products::reduce...>;

    template<typename T>
    using factor = ratio<
      T,
      (long)(Product1::template factor<T>::denominator * (... * Products::template factor<T>::denominator)),
      (long)(Product1::template factor<T>::numerator * (... * Products::template factor<T>::numerator))
    >;

    UNIT_SYMBOL(Product1::symbol() + (... + ("*" + Products::symbol())))
  };
}