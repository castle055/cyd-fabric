// Copyright (c) 2024, Victor Castillo, All rights reserved.

#include <cassert>

#include "common.h"

#include "cyd_fabric/refl/refl.h"
using namespace cyd::fabric;


void setup() {
}

struct test_struct: refl<test_struct> {
  field_t<int> i_val {this};
  field_t<double> d_val {this};
};

void print_obj(const refl<> &obj) {
  obj.with_type<test_struct>().transform([](const test_struct* ts) {
    return ts;
  });

    std::cout << "{" << std::endl;
  for (auto &field: obj.fields()) {
    std::cout << "something" << std::endl;
  }
  std::cout << "}" << std::endl;
}

//@formatter:off

TEST("Nominal Start-up") (
  test_struct ts{};
  test_struct tt{};
  tt.i_val = 2;
  tt.d_val = 2.45;

  ts.d_val = tt.d_val;
  ts.i_val = 100*tt.i_val / 3;

  print_obj(ts);

  for (field_base_t* field: ts.fields()) {
    field;
  }
  return 0;
)

TEST("Nominal Start-up2") (
  return 0;
)

TEST("Nominal Start-up3") (
  return 0;
)

//@formatter:on
