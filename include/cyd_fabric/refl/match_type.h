//
// Created by castle on 4/16/24.
//

#ifndef MATCH_TYPE_H
#define MATCH_TYPE_H

#include "cyd_fabric/templates/non_unique_variant.h"
#include "cyd_fabric/templates/common.h"
#include "cyd_fabric/templates/functor_arguments.h"

#include <optional>

namespace cyd::fabric {
  template<typename T>
  struct dis_result_t {
    using value_type = T;
    T val;
  };

  template<>
  struct dis_result_t<void> {
    using value_type = void;
  };

  template<typename T, typename FF>
  auto type_match_case(bool &matched, T &&val, FF &f) {
    if constexpr (non_template_functor<FF>) {
      if constexpr (std::is_same_v<T &, typename first_argument<FF>::type>) {
        if (!matched) {
          matched = true;
          if constexpr (std::is_void_v<decltype(f(val))>) {
            f(val);
            return std::optional {dis_result_t<void> { }};
          } else {
            return std::optional {dis_result_t {f(val)}};
          }
        } else {
          if constexpr (std::is_void_v<decltype(f(val))>) {
            return std::optional {dis_result_t<void> { }};
          } else {
            return std::optional<dis_result_t<decltype(f(val))>> {std::nullopt};
          }
        }
      } else if constexpr (std::is_void_v<typename first_argument<FF>::type>) {
        if (!matched) {
          matched = true;
          if constexpr (std::is_void_v<decltype(f())>) {
            f();
            return std::optional {dis_result_t<void> { }};
          } else {
            return std::optional {dis_result_t {f()}};
          }
        } else {
          if constexpr (std::is_void_v<decltype(f())>) {
            return std::optional {dis_result_t<void> { }};
          } else {
            return std::optional<dis_result_t<decltype(f())>> {std::nullopt};
          }
        }
      } else {
        if constexpr (std::is_void_v<decltype(f(std::declval<typename first_argument<FF>::type>()))>) {
          return std::optional {dis_result_t<void> { }};
        } else {
          return std::optional<dis_result_t<decltype(f(std::declval<typename first_argument<FF>::type>()))>> {
            std::nullopt
          };
        }
      }
    } else if constexpr (requires { f(val); }) {
      if (!matched) {
        matched = true;
        if constexpr (std::is_void_v<decltype(f(val))>) {
          f(val);
          return std::optional {dis_result_t<void> { }};
        } else {
          return std::optional {dis_result_t {f(val)}};
        }
      } else {
        if constexpr (std::is_void_v<decltype(f(val))>) {
          return std::optional {dis_result_t<void> { }};
        } else {
          return std::optional<dis_result_t<decltype(f(val))>> {std::nullopt};
        }
      }
    } else {
      return std::optional {dis_result_t<void> { }};
    }
  }

  template<typename T, typename... F>
  using dispatch_return_t = non_unique_variant_t<get_value_type<get_value_type<
    std::invoke_result_t<decltype(&type_match_case<T, F>), bool &, T &&, F &>
  >>...>;

  template<typename T, typename... F>
  auto match_type(T &&val, F &&... fs) {
    //-> dispatch_return_t<T,F...> {
    bool matched = false;

    if constexpr (!std::is_void_v<dispatch_return_t<T, F...>>) {
      dispatch_return_t<T, F...> res;

      ([&]<typename R> requires (!std::is_void_v<R>)(R &&r) {
        if (r.has_value()) {
          if constexpr (!std::is_void_v<typename R::value_type::value_type>) {
            res = std::move(r.value().val);
          }
        }
      }(type_match_case(matched, val, fs)), ...);

      return res;
    } else {
      ((type_match_case(matched, val, fs)), ...);
    }
  }

  template<typename T, typename... F>
  auto match_variant(T&& variant, F&&... fs) {
    std::visit([&](auto&& val) { match_type(val, std::forward<F&&>(fs)...); }, variant);
  }
}

#endif //MATCH_TYPE_H
