//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/units_preface.h>

export module fabric.units:concepts;
import :preface;

namespace cyd::fabric::units {
  export template<typename U_FROM, typename U_TO, typename T>
  concept Convertible = requires { unit_conversion_t<U_FROM, U_TO, T>::factor; }
                        || requires { unit_conversion_t<U_TO, U_FROM, T>::factor; };

  export template<typename U1, typename U2>
  concept SameScale = std::same_as<typename U1::scale, typename U2::scale>;

  export template<typename Q, typename S>
  concept Quantity = std::same_as<typename Q::unit::scale, S>;
}
