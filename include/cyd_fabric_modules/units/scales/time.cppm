//
// Created by castle on 5/11/24.
//
module;
#include "cyd_fabric_modules/headers/units_includes.h"
export module fabric.units.scales:time;
export import fabric.units;

export namespace cyd::fabric::units {
  SCALE(time, {
    UNIT(seconds, "s", 1,1)

    UNIT(milliseconds, "ms", 1,1000)
    UNIT(microseconds, "us", 1,1000000)
    UNIT(nanoseconds, "ns", 1,1000000000)

    UNIT(minutes, "min", 60,1)
    UNIT(hours, "hours", minutes::factor<T>::numerator*60,1)
    UNIT(days, "days", hours::factor<T>::numerator*24,1)
    UNIT(months, "months", days::factor<T>::numerator*30,1)
    UNIT(years, "years", days::factor<T>::numerator*365,1)
    UNIT(decades, "decades", years::factor<T>::numerator*10,1)
    UNIT(centuries, "centuries", decades::factor<T>::numerator*10,1)
    UNIT(millennia, "millennia", centuries::factor<T>::numerator*10,1)
  })
}
