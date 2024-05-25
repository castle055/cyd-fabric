// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef COMMON_H
#define COMMON_H


namespace cyd::fabric {
  template<typename T>
  using get_value_type = typename T::value_type;
}

#endif //COMMON_H
