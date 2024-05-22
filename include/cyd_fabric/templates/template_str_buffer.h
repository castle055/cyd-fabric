//
// Created by castle on 6/17/23.
//

#ifndef CYD_UI_TEMPLATE_STR_BUFFER_H
#define CYD_UI_TEMPLATE_STR_BUFFER_H

#include "cyd_fabric/type_aliases.h"

/**
 * Used in consteval literal operators implemented for strings, that way both the length and
 * the contends of the string can be used with static_assert to raise compile-time errors.
 *
 * CYD-UI uses this type for color declaration:
 *
 *   auto c  = "#FCAE1E"_color; // Ok
 *   auto c1 = "#FCAE1Eaaaa"_color;  // Compile-time error, invalid length
 *
 * @tparam length
 */
template<std::size_t length>
struct template_str_buffer {

  // Needs to be implicit if it's to work with literal operators
  consteval template_str_buffer(const char(&str)[length]) noexcept {
    for (std::size_t i = 0; i < length; ++i) {
      data[i] = str[i];
    }
  }

  char data[length] = {0};
  constexpr static size_t count = length - sizeof(char);
};

#endif //CYD_UI_TEMPLATE_STR_BUFFER_H
