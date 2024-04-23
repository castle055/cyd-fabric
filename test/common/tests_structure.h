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

#ifndef CYD_UI_TESTS_STRUCTURE_H
#define CYD_UI_TESTS_STRUCTURE_H

#include <unordered_map>
#include <string>
#include <functional>
#include <utility>
#include <iostream>
#include <cassert>

class test_case_t;

class test_case_list_t {
public:
  std::unordered_map<std::string, test_case_t &> test_cases { };
};

test_case_list_t CASES { };

class test_case_t {
public:
  std::string name;
  std::function<int()> body = []() { return 1; };

  test_case_t(const std::string &name_, std::function<int()> body_): name(name_), body(std::move(body_)) {
    CASES.test_cases.emplace(std::pair<std::string, test_case_t &> {name_, *this});
  }
};

#define TEST_ID case_
#define TEST_ID_NUM __LINE__
#define CONCAT_IMPL(A, B) A##B
#define CONCAT(A, B) CONCAT_IMPL(A, B)

#define __TEST_BODY_DECL__(...) []() { \
  __VA_ARGS__                          \
}};

#define TEST(NAME) \
test_case_t CONCAT(TEST_ID,TEST_ID_NUM) { \
  NAME,            \
  __TEST_BODY_DECL__

void setup();

int main(int argc, char* argv[]) {
  if (argc != 2) return 1;
  std::string name {argv[1]};

  setup();
  return CASES.test_cases.at(name).body();
}

#endif //CYD_UI_TESTS_STRUCTURE_H
