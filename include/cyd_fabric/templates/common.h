//
// Created by castle on 4/16/24.
//

#ifndef COMMON_H
#define COMMON_H


namespace cyd::fabric {
  template<typename T>
  using get_value_type = typename T::value_type;
}

#endif //COMMON_H
