/*!
 ! Copyright (c) 2024, Víctor Castillo Agüero.
 ! This file is part of the Cydonia project.
 !
 ! This library is free software: you can redistribute it and/or modify
 ! it under the terms of the GNU General Public License as published by
 ! the Free Software Foundation, either version 3 of the License, or
 ! (at your option) any later version.
 !
 ! This library is distributed in the hope that it will be useful,
 ! but WITHOUT ANY WARRANTY; without even the implied warranty of
 ! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ! GNU General Public License for more details.
 !
 ! You should have received a copy of the GNU General Public License
 ! along with this library.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cassert>

#include "common.h"

#include "cyd_fabric/refl/refl.h"

#include <ranges>
#include <algorithm>

#include "cyd_fabric/refl/match_type.h"
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

TEST("Nominal Start-up") {
  test_struct ts { };
  test_struct tt { };
  tt.i_val = 2;
  tt.d_val = 2.45;

  ts.d_val = tt.d_val;
  ts.i_val = 100 * tt.i_val / 3;

  print_obj(ts);

  for (field_base_t* field: ts.fields()) {
    field;
  }
  return 0;
}

static void print_quantity(const std::string &label, int quantity) {
  std::cout
    << label
    << ": "
    << std::to_string(quantity / 100)
    << "."
    << std::to_string(quantity % 100)
    << " €"
    << std::endl;
}

TEST("Nominal Start-up2") {
  std::vector<int> prices {269, 279, 259, 369, 538, 99, 199, 255, 155, 279, 95, 345, 485, 179, 148, 94, 115};

  int sum = 0;
  for (int p: prices) {
    sum += p;
  }

  print_quantity("TOTAL", sum);
  auto rs = std::ranges::sort(prices, std::ranges::greater { }, [](auto &&it) { return 1 / it; });
  sum     = 0;
  for (const int &p: prices) {
    sum += p;
    // print_quantity("", sum);
    print_quantity("", p);
  }

  return 0;
}


template<typename VALUE>
std::string print_type(VALUE &&val) {
  std::string type;
  match_type(val,
             [&]<typename T>(std::vector<T> &i) { type = "vector " + print_type(i[0]); },
             [&]<typename K, typename V>(std::unordered_map<K, V> &i) { type = "map"; },
             [&](int &i) { type = "int"; },
             [&](double &i) { type = "double"; },
             [&](long &i) { type = "long"; },
             [&](std::string &i) {
               type = "std::string";
               return 2.0f;
             },
             [&](float &i) { type = "float"; },
             [&] { type = "default"; }
  );
  return type;
}

template<typename VALUE>
std::string print_variant(VALUE &&val) {
  std::string type;
  match_variant(val,
                [&]<typename T>(std::vector<T> &i) { type = "vector " + print_type(i[0]); },
                [&]<typename K, typename V>(std::unordered_map<K, V> &i) { type = "map"; },
                [&](int &i) { type = "int"; },
                [&](double &i) { type = "double"; },
                [&](long &i) { type = "long"; },
                [&](std::string &i) {
                  type = "std::string";
                  return 2.0f;
                },
                [&](float &i) { type = "float"; },
                [&] { type = "default"; }
  );
  return type;
}

TEST("Nominal Start-up3") {
  non_unique_variant_t<int, int, long> vb { };
  vb = 12;
  std::visit([&](auto &&v) {
    assert(print_type(v) == "int");
  }, vb);
  assert(print_variant(vb) == "int");

  assert(print_type(123.23f) == "float");
  assert(print_type(std::vector<int>{1, 2, 3}) == "vector int");
  assert(print_type(std::vector<std::vector<int>>{{1, 2, 3}}) == "vector vector int");
  return 0;
}
