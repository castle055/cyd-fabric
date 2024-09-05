// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later
module;

//! Syntax sugar - Apply reduce
#define R(...) reduce<__VA_ARGS__>

export module fabric.units.core:quantity;
export import std;
import :preface;
import :concepts;
export import :frac;
export import :mul;

export namespace fabric::units {
  template<typename U, typename T>
  struct quantity_t {
    using unit      = U;
    using data_type = T;

    T value { };


    template<typename U1>
      requires SameScale<U, U1>
    auto operator<=>(const quantity_t<U1, T>& rhl) const {
      return this->value <=> rhl.template as<U>().value;
    }

    template<typename U1>
      requires SameScale<U, U1>
    bool operator<(const quantity_t<U1, T>& rhl) const {
      return this->value == rhl.template as<U>().value;
    }

    template<typename U1>
      requires SameScale<U, U1>
    bool operator==(const quantity_t<U1, T>& rhl) const {
      return this->value == rhl.template as<U>().value;
    }

    template<typename U1>
      requires SameScale<U, U1>
    quantity_t<R(frac<R(U), R(U)>), T> operator/(const quantity_t<U1, T> &rhl) const {
      return {this->value / rhl.template as<R(U)>().value};
    }

    template<typename U1>
      requires (!SameScale<U, U1>)
    quantity_t<R(frac<R(U), R(U1)>), T> operator/(const quantity_t<U1, T> &rhl) const {
      return {this->value / rhl.value};
    }

    template<typename U1>
      requires SameScale<U, U1>
    quantity_t<R(mul<R(U), R(U)>), T> operator*(const quantity_t<U1, T> &rhl) const {
      return {this->value * rhl.template as<R(U)>().value};
    }

    template<typename U1>
      requires (!SameScale<U, U1>)
    quantity_t<R(mul<R(U), R(U1)>), T> operator*(const quantity_t<U1, T> &rhl) const {
      return {this->value * rhl.value};
    }

    template<typename U1, typename T1>
      requires (SameScale<U, U1>)
    quantity_t<R(U), T1> operator+(const quantity_t<U1, T1> &rhl) const {
      return {this->value + rhl.template as<R(U)>().value};
    }

    template<typename U1, typename T1>
      requires (SameScale<U, U1>)
    quantity_t<R(U), T1> operator-(const quantity_t<U1, T1> &rhl) const {
      return {this->value - rhl.template as<R(U)>().value};
    }

    quantity_t<U, T> operator-() const {
      return {-(this->value)};
    }

    constexpr quantity_t() = default;

    constexpr quantity_t(T value_): value(value_) {
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
      requires SameScale<U, U1> || Convertible<U, U1, T> || ConvertibleScales<typename U::scale, typename U1::scale, U, U1, T>
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
      // } else if constexpr (requires { scale_conversion_t<typename U::scale, typename U1::scale>::template forward<U, U1, T>(*this); }) {
      //   return scale_conversion_t<typename U::scale, typename U1::scale>::template forward<U, U1, T>(*this);
      // } else if constexpr (requires { scale_conversion_t<typename U1::scale, typename U::scale>::template backward<U, U1, T>(*this); }) {
      //   return scale_conversion_t<typename U1::scale, typename U::scale>::template backward<U, U1, T>(*this);
      } else if constexpr (requires { scale_conversion_t<typename U::scale, typename U1::scale>{}; }) {
        return scale_conversion_t<typename U::scale, typename U1::scale>::template forward<U, U1, T>(*this);
      } else if constexpr (requires { scale_conversion_t<typename U1::scale, typename U::scale>{}; }) {
        return scale_conversion_t<typename U1::scale, typename U::scale>::template backward<U, U1, T>(*this);
      } else if constexpr (requires { unit_conversion_t<U, U1, T>::factor; }) {
        return {this->value * unit_conversion_t<U, U1, T>::factor};
      } else if constexpr (requires { unit_conversion_t<U1, U, T>::factor; }) {
        return {this->value / unit_conversion_t<U1, U, T>::factor};
      } else {
        static_assert(false, "No conversion factor available between these units.");
        return {0};
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
std::ostream &operator<<(std::ostream &o, const fabric::units::quantity_t<U, T> &quantity) {
  o << quantity.to_string();
  return o;
}
