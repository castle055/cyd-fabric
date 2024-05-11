//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/units_preface.h>
export module fabric.units:frac;
export import fabric.templates.ratio;

namespace cyd::fabric::units {
  export template<typename Numerator, typename Denominator>
  struct frac {
    using scale  = frac<typename Numerator::scale, typename Denominator::scale>;
    using reduce = frac<typename Numerator::reduce, typename Denominator::reduce>;

    template<typename T>
    using factor = ratio<
      T,
      (long)(Numerator::template factor<T>::denominator * Denominator::template factor<T>::numerator),
      (long)(Numerator::template factor<T>::numerator * Denominator::template factor<T>::denominator)
    >;

    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator::symbol() + ")")
  };
}
