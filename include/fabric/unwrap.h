// Copyright (c) 2026, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_FABRIC_UNWRAP_H
#define CYD_FABRIC_UNWRAP_H

#define unwrap(...)                                                                                \
  ({                                                                                               \
    auto&& __$_tmp_result__ = (__VA_ARGS__);                                                       \
    static_assert(                                                                                 \
      not std::is_void_v<                                                                          \
        typename std::remove_reference_t<decltype(__$_tmp_result__)>::result_type>,                \
      "Cannot unwrap void result"                                                                  \
    );                                                                                             \
    if (not __$_tmp_result__.ok())                                                                 \
      return __$_tmp_result__.error();                                                             \
    std::move(__$_tmp_result__.value());                                                           \
  })

#define co_unwrap(...)                                                                             \
  ({                                                                                               \
    auto&& __$_tmp_result__ = (__VA_ARGS__);                                                       \
    static_assert(                                                                                 \
      not std::is_void_v<                                                                          \
        typename std::remove_reference_t<decltype(__$_tmp_result__)>::result_type>,                \
      "Cannot unwrap void result"                                                                  \
    );                                                                                             \
    if (not __$_tmp_result__.ok())                                                                 \
      co_return __$_tmp_result__.error();                                                          \
    std::move(__$_tmp_result__.value());                                                           \
  })

#endif // CYD_FABRIC_UNWRAP_H
