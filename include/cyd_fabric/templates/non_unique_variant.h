// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NON_UNIQUE_VARIANT_H
#define NON_UNIQUE_VARIANT_H

#include <variant>

namespace cyd::fabric {
  template<int N, typename...>
  struct internal_non_unique_variant;

  template<int N>
  struct internal_non_unique_variant<N> {
    using type = void;
  };

  template<int N, typename T, typename... TRest>
    requires (N < 0)
  struct internal_non_unique_variant<N, T, TRest...> {
    using type = std::variant<T, TRest...>;
  };

  template<int N, typename T, typename... TRest>
    requires (N >= 0)
  struct internal_non_unique_variant<N, T, TRest...> {
    using type = std::conditional_t<(N > 0),
                                    std::conditional_t<(std::is_void_v<T> || (std::is_same_v<T, TRest> || ...)),
                                                       typename internal_non_unique_variant<N - 1, TRest...>::type,
                                                       typename internal_non_unique_variant<N - 1, TRest..., T>::type>,
                                    std::variant<T, TRest...>>;
  };

  template<typename... TRest>
  using non_unique_variant_t = typename internal_non_unique_variant<sizeof...(TRest), TRest...>::type;
}

#endif //NON_UNIQUE_VARIANT_H
