// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  concepts.cppm
 *! \brief 
 *!
 */

export module fabric.linalg:concepts;

import std;

export
{
  template<typename F, typename FT>
  struct valid_fun {
    constexpr static bool valid = false;
  };

  template<typename F, typename Ret, typename... Args>
  struct valid_fun<F, Ret(Args...)> {
    constexpr static bool valid = std::convertible_to<std::invoke_result<F, Args...>, Ret>;
  };

  template<typename F, typename FT>
  inline constexpr bool valid_fun_v = valid_fun<F, FT>::valid;

  template<typename F, typename FT>
  concept Fun = requires { valid_fun_v<F, FT>; };
}

