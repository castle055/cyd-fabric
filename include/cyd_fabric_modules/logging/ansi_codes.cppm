// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ansi_codes.cppm
 *! \brief
 *!
 */
module;
#define STR(X) #X
#define CODE(X) STR(\033[X## m)

export module fabric.logging.ansi_codes;

import std;
import reflect;

export namespace fabric::ansi {
  const char* reset     = CODE(0);
  const char* dim       = CODE(2);
  const char* underline = CODE(24);

  std::string color256(unsigned int num) {
    return std::format("\033[38;{}m", num);
  }

  std::string bg256(unsigned int num) {
    return std::format("\033[48;{}m", num);
  }

  namespace color {
    const char* black    = CODE(30);
    const char* red      = CODE(31);
    const char* green    = CODE(32);
    const char* yellow   = CODE(33);
    const char* blue     = CODE(34);
    const char* magenta  = CODE(35);
    const char* cyan     = CODE(36);
    const char* white    = CODE(37);
    const char* default_ = CODE(39);

    const char* black_bri   = CODE(90);
    const char* red_bri     = CODE(91);
    const char* green_bri   = CODE(92);
    const char* yellow_bri  = CODE(93);
    const char* blue_bri    = CODE(94);
    const char* magenta_bri = CODE(95);
    const char* cyan_bri    = CODE(96);
    const char* white_bri   = CODE(97);
    const char* default_bri = CODE(99);
  } // namespace color

  namespace background {
    const char* black    = CODE(40);
    const char* red      = CODE(41);
    const char* green    = CODE(42);
    const char* yellow   = CODE(43);
    const char* blue     = CODE(44);
    const char* magenta  = CODE(45);
    const char* cyan     = CODE(46);
    const char* white    = CODE(47);
    const char* default_ = CODE(49);

    const char* black_bri   = CODE(100);
    const char* red_bri     = CODE(101);
    const char* green_bri   = CODE(102);
    const char* yellow_bri  = CODE(103);
    const char* blue_bri    = CODE(104);
    const char* magenta_bri = CODE(105);
    const char* cyan_bri    = CODE(106);
    const char* white_bri   = CODE(107);
    const char* default_bri = CODE(109);
  } // namespace background
} // namespace fabric::ansi
