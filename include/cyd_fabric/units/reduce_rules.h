// Copyright (c) 2024, Victor Castillo, All rights reserved.

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
