//
// Created by castle on 5/11/24.
//

#ifndef UNITS_PREFACE_H
#define UNITS_PREFACE_H

#include <string>

#define UNIT_SYMBOL(...) static constexpr inline std::string symbol() noexcept { return (__VA_ARGS__); }

#endif //UNITS_PREFACE_H
