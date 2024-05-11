//
// Created by castle on 5/11/24.
//

export module fabric.templates.ratio;

namespace cyd::fabric {
  export template<typename T, auto Num, auto Den>
  struct ratio {
    static constexpr T numerator   = Num;
    static constexpr T denominator = Den;
  };
}
