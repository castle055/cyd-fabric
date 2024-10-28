// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef XDG_H
#define XDG_H

#include "env.h"

namespace cyd::fabric::xdg {
#define XDG_PATH(NAME, ...) ENV(NAME, XDG_##NAME##_HOME, __VA_ARGS__)

  XDG_PATH(CONFIG, env::HOME() + "/.config")
  XDG_PATH(CACHE, env::HOME() + "/.cache")
  XDG_PATH(DATA, env::HOME() + "/.local/share")
}

#endif //XDG_H
