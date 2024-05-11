//
// Created by castle on 5/11/24.
//

export module fabric.units:preface;

namespace cyd::fabric::units {
  export struct no_unit {
    using reduce = no_unit;
  };

  export template<typename U_FROM, typename U_TO, typename T>
  struct unit_conversion_t {
  };
}
