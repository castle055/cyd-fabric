// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_ASSERT_H
#define CYD_UI_ASSERT_H

#define STATIC_ASSERT_IS_HEX(CHAR) \
      static_assert(((CHAR) >= 'A' && (CHAR) <= 'F') \
|| ((CHAR) >= 'a' && (CHAR) <= 'f') \
|| ((CHAR) >= '0' && (CHAR) <= '9'), \
"Invalid char, must be hexadecimal")

#endif //CYD_UI_ASSERT_H
