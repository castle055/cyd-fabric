// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

//
// Created by castle on 5/11/24.
//

#ifndef UNITS_PREFACE_H
#define UNITS_PREFACE_H

#include <string>

#include <cyd_fabric/ts/packs.h>

#define UNIT_SYMBOL(...) static constexpr inline std::string symbol() noexcept { return (__VA_ARGS__); }

namespace cyd::fabric::units {
}

#endif //UNITS_PREFACE_H
