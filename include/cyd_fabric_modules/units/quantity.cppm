//
// Created by castle on 5/11/24.
//
module;
#include <cyd_fabric_modules/headers/units_preface.h>
#include <typeinfo>
export module fabric.units:quantity;
import :concepts;
export import :frac;
export import :mul;

namespace cyd::fabric::units {
  export template<typename U, typename T>
  struct quantity_t {
    using unit      = U;
    using data_type = T;

    T value { };

    template<typename U1>
    quantity_t<typename frac<U, U1>::reduce, T> operator/(const quantity_t<U1, T> &rhl) const {
      return {this->value / rhl.value};
    }

    template<typename U1>
      requires SameScale<U, U1>
    quantity_t<typename mul<U, U>::reduce, T> operator*(const quantity_t<U1, T> &rhl) const {
      return {this->value * rhl.template as<U>().value};
    }

    template<typename U1>
      requires (!SameScale<U, U1>)
    quantity_t<typename mul<U, U1>::reduce, T> operator*(const quantity_t<U1, T> &rhl) const {
      return {this->value * rhl.value};
    }

    template<typename U1, typename T1>
      requires (SameScale<U, U1>)
    quantity_t<typename U::reduce, T1> operator+(const quantity_t<U1, T1> &rhl) const {
      return {this->value + rhl.template as<U>().value};
    }

    template<typename U1, typename T1>
      requires (SameScale<U, U1>)
    quantity_t<typename U::reduce, T1> operator-(const quantity_t<U1, T1> &rhl) const {
      return {this->value - rhl.template as<U>().value};
    }

    quantity_t() = default;

    quantity_t(T value_): value(value_) {
    }

    quantity_t(const quantity_t &other) {
      this->value = other.value;
    }

    quantity_t(quantity_t &&other) noexcept {
      this->value = other.value;
    }

    quantity_t &operator=(const quantity_t &rhl) {
      this->value = rhl.value;
      return *this;
    }

    quantity_t &operator=(quantity_t &&rhl) noexcept {
      this->value = rhl.value;
      return *this;
    }


    template<typename U1>
      requires SameScale<U, U1> || Convertible<U, U1, T>
    [[nodiscard]]
    quantity_t<U1, T> as() const {
      if constexpr (std::is_same_v<U, U1>) {
        return *this;
      } else if constexpr (SameScale<U, U1>) {
        return quantity_t<U1, T> {
          this->value
          * (U1::template factor<T>::denominator
             * U::template factor<T>::numerator)
          / (U1::template factor<T>::numerator
             * U::template factor<T>::denominator)
        };
      } else if constexpr (requires { unit_conversion_t<U, U1, T>::factor; }) {
        return {this->value * unit_conversion_t<U, U1, T>::factor};
      } else if constexpr (requires { unit_conversion_t<U1, U, T>::factor; }) {
        return {this->value / unit_conversion_t<U1, U, T>::factor};
      } else {
        static_assert(false, "No conversion factor available for these units.");
      }
    }

    [[nodiscard]]
    inline std::string to_string() const noexcept {
      std::string str { };
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

export template<typename U, typename T>
std::ostream &operator<<(std::ostream &o, const cyd::fabric::units::quantity_t<U, T> &quantity) {
  o << quantity.to_string();
  return o;
}
