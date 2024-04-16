//
// Created by castle on 4/16/24.
//

#ifndef TYPE_ALIASES_H
#define TYPE_ALIASES_H

#include <string>
#include <vector>

namespace cyd::fabric::type_aliases {
  using u8  = unsigned char;
  using u16 = unsigned short;
  using u32 = unsigned int;
  using u64 = unsigned long;

  using i8  = char;
  using i16 = short;
  using i32 = int;
  using i64 = long;

  using f32 = float;
  using f64 = double;

  using str = std::string;

  template<typename T>
  using vec = std::vector<T>;
}

#endif //TYPE_ALIASES_H
