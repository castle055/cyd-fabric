// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.templates.str_buffer;

import fabric.type_aliases;

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
export template<u64 length>
struct template_str_buffer {

  // Needs to be implicit if it's to work with literal operators
  consteval template_str_buffer(const char(&str)[length]) noexcept {
    for (u64 i = 0; i < length; ++i) {
      data[i] = str[i];
    }
  }

  char data[length] = {0};
  constexpr static u64 count = length - sizeof(char);
};
