// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  _concepts.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:concepts;

import std;

export namespace fabric::async {
  template<class T>
  concept EventType = requires
  {
    { T::type } -> std::convertible_to<std::string>;
  };
}