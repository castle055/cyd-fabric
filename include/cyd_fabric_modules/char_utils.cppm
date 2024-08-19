// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.char_utils;

// #define STATIC_ASSERT_IS_HEX(CHAR) \
//       static_assert(((CHAR) >= 'A' && (CHAR) <= 'F') \
// || ((CHAR) >= 'a' && (CHAR) <= 'f') \
// || ((CHAR) >= '0' && (CHAR) <= '9'), \
// "Invalid char, must be hexadecimal")

export consteval bool char_is_hex(const char c) {
      return ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f') || ((c) >= '0' && (c) <= '9');
}

export template <char C>
consteval void assert_char_is_hex() {
      static_assert(char_is_hex(C), "Invalid char, must be hexadecimal");
}
