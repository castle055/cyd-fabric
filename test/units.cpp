// Copyright (c) 2024, Victor Castillo, All rights reserved.

#include "common.h"
#include <cassert>

#include <chrono>

#include "cyd_fabric/units/units.h"


void setup() {
}

TEST("Nominal Start-up") (
  asdfasdfasdf();
  return 0;
)

TEST("Factors") (
  using namespace cyd::fabric::units;
  
  quantity_t<distance::meters, double> d1{10};
  quantity_t<distance::kilometers, double> d2{1};
  
  std::cout << "d1: " << (d1) << std::endl;
  std::cout << "d1: " << (d1.as<distance::kilometers>()) << std::endl;
  std::cout << "d2: " << (d2) << std::endl;
  std::cout << "d2: " << (d2.as<distance::meters>()) << std::endl;
  std::cout << " *: " << (d1*d2) << std::endl;
  
  
  return 0;
)
