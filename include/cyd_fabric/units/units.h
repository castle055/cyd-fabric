// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_FABRIC_UNITS_H
#define CYD_FABRIC_UNITS_H

#include <type_traits>
#include <typeinfo>
#include <string>


#include "cyd_fabric/templates/ratio.h"

namespace cyd::fabric::units {
  struct no_unit {
    using reduce = no_unit;
  };
  
  template <typename U_FROM, typename U_TO, typename T>
  struct unit_conversion_t {};
  
  template <typename U_FROM, typename U_TO, typename T>
  concept Convertible = requires { unit_conversion_t<U_FROM,U_TO,T>::factor; }
                        || requires { unit_conversion_t<U_TO,U_FROM,T>::factor; };


#define UNIT_SYMBOL(...) static constexpr inline std::string symbol() noexcept { return (__VA_ARGS__); }
  
  template<typename Numerator, typename Denominator>
  struct frac {
    using scale = frac<typename Numerator::scale, typename Denominator::scale>;
    using reduce = frac<typename Numerator::reduce, typename Denominator::reduce>;
    
    template <typename T>
    using factor = ratio<
      T,
      (int)(Numerator::template factor<T>::denominator * Denominator::template factor<T>::numerator),
      (int)(Numerator::template factor<T>::numerator * Denominator::template factor<T>::denominator)
    >;
    
    UNIT_SYMBOL("(" + Numerator::symbol() + ")/(" + Denominator::symbol() + ")")
  };
  
  template<typename Product1, typename ...Products>
  struct mul {
    using scale = mul<typename Product1::scale, typename Products::scale...>;
    using reduce = mul<typename Product1::reduce, typename Products::reduce...>;
    
    template <typename T>
    using factor = ratio<
      T,
      (int)(Product1::template factor<T>::denominator * (... * Products::template factor<T>::denominator)),
      (int)(Product1::template factor<T>::numerator * (... * Products::template factor<T>::numerator))
    >;
    
    UNIT_SYMBOL(Product1::symbol() + (... + ("*" + Products::symbol())))
  };
  
  template <typename U1, typename U2>
  concept SameScale = std::same_as<typename U1::scale, typename U2::scale>;
  
  template <typename Q, typename S>
  concept Quantity = std::same_as<typename Q::unit::scale, S>;
  
  template <typename U, typename T>
  struct quantity_t {
    using unit = U;
    using data_type = T;
    
    T value{};
    
    template <typename U1>
    quantity_t<typename frac<U,U1>::reduce,T> operator/(const quantity_t<U1,T>& rhl) const {
      return {this->value/rhl.value};
    }
    
    template <typename U1>
    requires SameScale<U,U1>
    quantity_t<typename mul<U,U>::reduce,T> operator*(const quantity_t<U1,T>& rhl) const {
      return {this->value * rhl.template as<U>().value};
    }
    
    template <typename U1>
    requires (!SameScale<U,U1>)
    quantity_t<typename mul<U,U1>::reduce,T> operator*(const quantity_t<U1,T>& rhl) const {
      return {this->value * rhl.value};
    }
    
    template <typename U1>
    requires SameScale<U,U1> || Convertible<U,U1,T>
    [[nodiscard]]
    quantity_t<U1, T> as() const {
      if constexpr (std::is_same_v<U,U1>) {
        return *this;
      } else if constexpr (SameScale<U,U1>) {
        return quantity_t<U1,T>{this->value
                                * (U1::template factor<T>::denominator
                                   * U::template factor<T>::numerator)
                                / (U1::template factor<T>::numerator
                                   * U::template factor<T>::denominator)};
      } else if constexpr (requires { unit_conversion_t<U,U1,T>::factor; }) {
        return {this->value * unit_conversion_t<U,U1,T>::factor};
      } else if constexpr (requires { unit_conversion_t<U1,U,T>::factor; }) {
        return {this->value / unit_conversion_t<U1,U,T>::factor};
      } else {
        static_assert(false, "No conversion factor available for these units.");
      }
    }
    
    [[nodiscard]]
    inline std::string to_string() const noexcept {
      std::string str{};
      str.append(std::to_string(value));
      str.append(" [");
      str.append(U::symbol());
      str.append(":");
      str.append(typeid(T).name());
      str.append("]");
      return str;
    }
  };
}

template<typename U, typename T>
std::ostream & operator<<(std::ostream &o, const cyd::fabric::units::quantity_t<U,T>& quantity) {
  o << quantity.to_string();
  return o;
}

#define SCALE(NAME, ...) \
namespace NAME {         \
  struct scale {};       \
  template <typename Q> concept quantity = Quantity<Q, scale>; \
}                        \
namespace NAME __VA_ARGS__            \

#define UNIT(NAME, SYMBOL, FACTOR_NUM, FACTOR_DEN) \
struct NAME {                                      \
  using scale = scale;                             \
  template <typename T>                            \
  using factor = cyd::fabric::ratio<T, FACTOR_NUM, FACTOR_DEN>; \
  using reduce = NAME;                             \
  UNIT_SYMBOL(SYMBOL)                              \
};

#include "reduce_rules.h"

#include "scales/distance_scale.h"
#include "scales/time_scale.h"

#include "scales/speed_scale.h"

#include <iostream>

using namespace cyd::fabric::units;

time::quantity auto compute_eta(
  const distance::quantity auto& distance,
  const speed::quantity auto& speed
) {
  return distance / speed;
}

void asdfasdfasdf() {
  quantity_t<distance::meters, double> distance{123000};
  quantity_t<speed::m_s, double> speed{20};
  quantity_t<time::seconds, double> ETA = compute_eta(distance, speed);
  std::cout << compute_eta(distance, speed) << std::endl;
  std::cout << distance*distance.as<distance::kilometers>() << std::endl;
  
  std::cout
    << speed << std::endl
    << (speed.as<frac<distance::kilometers,time::seconds>>() * ETA) << std::endl
  ;
  
//  quantity_t dkm = distance.as<kilometers>();
//  quantity_t ds = distance.as<seconds>();
  
  quantity_t<speed::m_s, double> avg_speed = distance / ETA;
}
#endif //CYD_FABRIC_UNITS_H
