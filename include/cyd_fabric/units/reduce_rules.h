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

#ifndef CYD_FABRIC_REDUCE_RULES_H
#define CYD_FABRIC_REDUCE_RULES_H

namespace cyd::fabric::units {
  
  template <typename Numerator, typename Denominator>
  struct frac<Numerator,frac<Numerator,Denominator>> {
    using reduce = typename Denominator::reduce;
  };
  template <typename Numerator1, typename Numerator2, typename Denominator>
  struct frac<Numerator1,frac<Numerator2,Denominator>> {
    using reduce = typename frac<mul<typename Numerator1::reduce,typename Denominator::reduce>, typename Numerator2::reduce>::reduce;
  };
  template <typename Numerator, typename Denominator1, typename Denominator2>
  struct frac<frac<Numerator,Denominator1>,Denominator2> {
    using reduce = typename frac<typename Numerator::reduce, mul<typename Denominator1::reduce,typename Denominator2::reduce>>::reduce;
  };
  template <typename Numerator>
  struct frac<Numerator,Numerator> {
    using reduce = no_unit;
  };
  template <typename N>
  struct frac<N, no_unit> {
    using reduce = typename N::reduce;
  };
  template <typename N>
  struct frac<no_unit, N> {
    using reduce = typename N::reduce;
  };
  
  
  
  template <typename P>
  struct mul<P, no_unit> {
    using reduce = typename P::reduce;
  };
  template <typename P>
  struct mul<no_unit, P> {
    using reduce = typename P::reduce;
  };
  template <typename P1, typename P2>
  struct mul<frac<P1,P2>,P2> {
    using reduce = typename P1::reduce;
  };
  template <typename P1, typename N, typename P2>
  struct mul<frac<P1,N>,P2> {
    using reduce = typename frac<mul<typename P1::reduce, typename P2::reduce>, typename N::reduce>::reduce;
  };
}

#endif //CYD_FABRIC_REDUCE_RULES_H
