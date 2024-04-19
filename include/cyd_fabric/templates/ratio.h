// Copyright (c) 2024, Victor Castillo, All rights reserved.

#ifndef CYD_FABRIC_RATIO_H
#define CYD_FABRIC_RATIO_H

namespace cyd::fabric {
  
  template <typename T, auto Num, auto Den>
  struct ratio {
    static constexpr T numerator = Num;
    static constexpr T denominator = Den;
  };
  
}

#endif //CYD_FABRIC_RATIO_H
