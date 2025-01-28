
// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

// #include <cassert>
#include "common.h"

import reflect;

import fabric.logging;
import packtl;
import reflect.serialize;

void setup() {
  LOG::INIT{}.filter()["stdout"];
}

template <typename T>
struct a {
  int aa = 1234;

  void asdf() {}

private:
  T wow;
};

struct bb {
  a<int>    a1{};
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

namespace charts {}

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
  refl::serializer<>::to_stream(std::cout, c{});

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

  return 0;
}

struct hola {
  int a = 4;
};

struct rt_test {
  int  a = 1;
  int  b = 2;
  int  c = 3;
  // std::string b;
  hola h;
};

TEST("Runtime Reflection") {
  auto rti = refl::type_info::from<rt_test>();

  rt_test test;
  for (const auto& field: rti.fields()) {
    void* f_ptr = field.get_ptr(&test);
    if (field.type().is_type<hola>()) {
      int& f_ref = field.get_ref<int>(&test);

      LOG::print{INFO
      }("Field: {}: [{}] [{}] = (0x{:X}) {};",
        field.offset,
        field.type().name(),
        field.name,
        (std::size_t)f_ptr,
        f_ref);
    }
  }

  return 0;
}

struct serialize_me {
  int           a    = 5;
  std::string   str  = "hello there!";
  serialize_me* next = nullptr;
};
TEST("Serialization") {
  serialize_me sm1{};
  serialize_me sm{};
  sm.next = &sm1;

  refl::serializer<formats::json_fmt>::to_stream(std::cout, sm);

  std::cout << refl::to_string<formats::json_fmt>(sm);

  return 0;
}


struct [[refl::annotation]] ignore {};

struct [[refl::annotation]] json_name {
  // consteval explicit json_name(std::string&& name_): name(name_) {}
  std::string name;
};

struct annotate_me {
  [[meta(ignore {})]]
  int a = 5;
  [[meta("asdf")]]
  [[meta(json_name {"hello"})]]
  std::string str = "hello there!";

  [[refl::ignore]]
  std::tuple<const char*> tt = {"asdf"};
};
TEST("Annotations") {
  annotate_me am{};
  // auto ann0 = std::get<0>(annotate_me::anns);
  // auto ann1 = std::get<1>(annotate_me::anns);

  // using info = refl::static_type_info<annotate_me>;
  // std::get<0>(ann1).name.c_str();
  std::cout << "WHAT?: " << refl::serializer<>::to_string(am) << std::endl;
  std::cout << "WHAT?: " << refl::serializer<formats::json_fmt>::to_string(refl::field<annotate_me, 1>::metadata<1>) << std::endl;
  // std::cout << "WHAT?: " << std::get<0>(refl::field<annotate_me, 1>::metadata) << std::endl;
  // std::cout << "WHAT?: " << refl::field_meta<annotate_me, 0>.name << std::endl;
  // std::cout << "WHAT?: " << info::field_names[0] << std::endl;
  // std::cout << "WHAT?: " << std::get<0>(std::get<2>(info::field_metadata)).name << std::endl;
  // refl::serializer<formats::json_fmt>::to_stream(std::cout, am);

  constexpr std::tuple<const char*> t = {"asdf"};
  return 0;
}
