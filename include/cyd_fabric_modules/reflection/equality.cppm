// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  pretty_print.cppm
 *! \brief
 *!
 */

export module reflect:equality;

import std;

export import fabric.ts.packs;

export import :types;
export import :accessors;


export template <refl::Reflected R>
bool operator==(const R& lhs, const R& rhs) {
  return false;
}
