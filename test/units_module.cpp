// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <cyd_fabric_modules/headers/units_includes.h>

#include "common.h"
#include <cassert>

#include <chrono>

// import fabric.units;
import fabric.units.scales;

using namespace cyd::fabric::units;
using namespace cyd::fabric::units::distance;
using namespace cyd::fabric::units::angle;
using namespace cyd::fabric::units::time;
using namespace cyd::fabric::units::mass;
using namespace cyd::fabric::units::volume;
using namespace cyd::fabric::units::speed;

//! I know this can be checked at compile-time, but for the purpose of
//! consistency, I want ALL tests to fail/succeed at run-time.
//! To be clear, the check is done at compile-time but reported at
//! run-time.
template<typename U1, typename U2>
inline void assert_reduces_to() {
  if constexpr (std::is_same_v<typename U1::reduce, U2>) {
    std::cout << "[PASS] [" << U1::symbol() << "] reduces to [" << U2::symbol() << "]" << std::endl;
  } else {
    std::cerr << "[FAIL] [" << U1::symbol() << "] reduces to [" << U1::reduce::symbol() << "] (expected [" <<
      U2::symbol() << "])" << std::endl;
    // __assert_fail(("[" + U1::symbol() + "] reduces to [" + U2::symbol() + "]").c_str(), __FILE_NAME__, __LINE__, __ASSERT_FUNCTION);
    std::abort();
  }
}

template<typename U1>
struct assert_unit {
  //! I know this can be checked at compile-time, but for the purpose of
  //! consistency, I want ALL tests to fail/succeed at run-time.
  //! To be clear, the check is done at compile-time but reported at
  //! run-time.
  template<typename U2>
  static void reduces_to() {
    if constexpr (std::is_same_v<typename U1::reduce, U2>) {
      std::cout << "[PASS] [" << U1::symbol() << "] reduces to [" << U2::symbol() << "]" << std::endl;
    } else {
      std::cerr << "[FAIL] [" << U1::symbol() << "] reduces to [" << U1::reduce::symbol() << "] (expected [" <<
        U2::symbol() << "])" << std::endl;
      // __assert_fail(("[" + U1::symbol() + "] reduces to [" + U2::symbol() + "]").c_str(), __FILE_NAME__, __LINE__, __ASSERT_FUNCTION);
      std::abort();
    }
  }
};

void setup() {
}

//@formatter:off
TEST("Nominal Start-up")(
  return 0;
)

TEST("Factors")(
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

TEST("Unit reduction")(
  assert_unit   <frac<meters, frac<meters, seconds>>>               ::reduces_to<seconds>();
  assert_unit   <frac<meters, frac<seconds, meters>>>               ::reduces_to<frac<mul<meters,meters>,seconds>>();
  assert_unit   <frac<frac<meters, seconds>, seconds>>              ::reduces_to<frac<meters, mul<seconds, seconds>>>();
  assert_unit   <frac<seconds, seconds>>                            ::reduces_to<no_unit>();
  assert_unit   <frac<seconds, no_unit>>                            ::reduces_to<seconds>();
  assert_unit   <frac<no_unit, seconds>>                            ::reduces_to<frac<no_unit, seconds>>();
  assert_unit   <frac<no_unit, no_unit>>                            ::reduces_to<no_unit>();
  assert_unit   <mul<no_unit, no_unit>>                             ::reduces_to<no_unit>();
  assert_unit   <mul<meters, no_unit>>                              ::reduces_to<meters>();
  assert_unit   <mul<no_unit, meters>>                              ::reduces_to<meters>();
  assert_unit   <mul<meters, frac<grams, meters>>>                  ::reduces_to<grams>();
  assert_unit   <mul<frac<grams, meters>, meters>>                  ::reduces_to<grams>();
  assert_unit   <mul<meters, frac<grams, seconds>>>                 ::reduces_to<frac<mul<meters, grams>, seconds>>();
  assert_unit   <mul<frac<grams, seconds>, meters>>                 ::reduces_to<frac<mul<meters, grams>, seconds>>();
  assert_unit   <frac<mul<meters, meters>, mul<meters, meters>>>    ::reduces_to<no_unit>();

  quantity_t<mul<meters, meters>, float> asd;
  quantity_t<frac<mul<meters, meters>,mul<meters, meters>>::reduce, float> fdsa = asd/asd;
  quantity_t<no_unit, float> fsa = asd/asd;

  fdsa;
  fsa;
  return 0;
)
//@formatter:on
