
// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

// #include <cassert>
#include "common.h"

import reflect;


void setup() {}

template <typename T>
struct a {
  int aa = 1234;
  long bb = 12345678;
  std::string name = "hello there!";
  char wtf = 'w';
  bool yes = true;

  void asdf() {}

private:
  T wow;
};

struct bb {
  a<int> a1{};
  a<double> a2{};
};

struct c {
  // bb& baby;
  struct test {
    int b;
  } props;
  int as;

void hello(int a) {}
private:
void hello(std::string a, int v) {}
};

namespace charts {
}

TEST("Nominal Start-up") {
  std::cout << "STARTING TEST" << std::endl;
  //    a::field_names;

  a<int> obj{};
  a<int> obj1{};
  // typename a<int>::__type_info__ fdsa{};

  // print_obj(obj);

  // auto str = refl::serializer<a<int>, refl::formats::reflected>::to_string(obj);

  // std::cout << str << std::endl;

  bb b{};
  refl::serializer<c, refl::formats::reflected>::to_stream(std::cout, c{});

  // bool eq = obj == obj1;
  // if (eq) {
  //   std::cout << "OBJECTS ARE EQUAL!" << std::endl;
  // } else {
  //   std::cout << "OBJECTS ARE NOT EQUAL!" << std::endl;
  // }

  // obj.name = "General kenobi...";

  // eq = obj == obj1;
  // if (eq) {
  //   std::cout << "OBJECTS ARE EQUAL!" << std::endl;
  // } else {
  //   std::cout << "OBJECTS ARE NOT EQUAL!" << std::endl;
  // }

  return 1;
}

