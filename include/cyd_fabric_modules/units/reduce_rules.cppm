// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#define TO(...)             \
{                           \
  using type = __VA_ARGS__; \
}

#define REDUCE_PATTERN(...) struct reduce_impl<__VA_ARGS__>

export module fabric.units:reduce_rules;
export import std;
export import fabric.ts.packs;
import :preface;

export namespace cyd::fabric::units {
  template <typename N>
  REDUCE_PATTERN(frac<N, N>) TO(no_unit);

  template <>
  REDUCE_PATTERN(frac<no_unit, no_unit>) TO(no_unit);

  template <typename N>
  REDUCE_PATTERN(frac<N, no_unit>) TO(reduce<N>);


  template<typename N, typename D>
  requires (!is_frac_v<N>)
  REDUCE_PATTERN(frac<N, frac<N, D>>) TO(reduce<D>);

  template<typename N, typename D1, typename D2>
  requires (!is_frac_v<N>)
  REDUCE_PATTERN(frac<N, frac<D1, D2>>) TO(reduce<frac<reduce<mul<reduce<N>,reduce<D2>>>,reduce<D1>>>);

  template<typename N, typename D>
  requires (!is_frac_v<N>)
  REDUCE_PATTERN(frac<frac<N, D>, N>) TO(reduce<frac<no_unit,reduce<D>>>);

  template<typename N, typename D1, typename D2>
  requires (!is_frac_v<N>)
  REDUCE_PATTERN(frac<frac<D1, D2>, N>) TO(reduce<frac<reduce<D1>, reduce<mul<reduce<D2>,reduce<N>>>>>);

  template<typename N1, typename N2, typename D1, typename D2>
  requires (!std::is_same_v<N1, D1> || !std::is_same_v<N2, D2>)
  REDUCE_PATTERN(frac<frac<N1, N2>, frac<D1, D2>>) TO(reduce<frac<reduce<mul<reduce<N1>, reduce<D2>>>, reduce<mul<reduce<D1>,reduce<N2>>>>>);


  template <typename N, typename ...Ds>
  requires ts::packs::Contains<N, Ds...>
  REDUCE_PATTERN(frac<N, mul<Ds...>>) TO(reduce<frac<no_unit, reduce<cancel_out<mul<Ds...>, N>>>>);

  template <typename D, typename ...Ns>
  requires ts::packs::Contains<D, Ns...>
  REDUCE_PATTERN(frac<mul<Ns...>, D>) TO(reduce<cancel_out<mul<Ns...>, D>>);

  template <typename M1, typename M2>
  requires (is_mul_v<M1> && is_mul_v<M2> && !ts::packs::share_items<M1, M2>::value)
  REDUCE_PATTERN(frac<M1, M2>) TO(frac<M1, M2>);

  template <typename M1, typename M2>
  requires (is_mul_v<M1> && is_mul_v<M2> && ts::packs::share_items<M1, M2>::value)
  REDUCE_PATTERN(frac<M1, M2>) TO(reduce<frac<reduce<typename cancel_out_many<M1, M2>::type>, reduce<typename cancel_out_many<M2,M1>::type>>>);


  template <>
  REDUCE_PATTERN(mul<>) TO(no_unit);

  template <typename P>
  REDUCE_PATTERN(mul<P>) TO(reduce<P>);

  template <typename ...Ps>
  requires (!ts::packs::Contains<no_unit, Ps...> && ((!is_mul_v<Ps> && !is_frac_v<Ps>) && ...))
  REDUCE_PATTERN(mul<Ps...>) TO(mul<Ps...>);

  template <typename ...Ps>
  requires (!ts::packs::Contains<no_unit, Ps...> && ((is_mul_v<Ps> || is_frac_v<Ps>) || ...))
  REDUCE_PATTERN(mul<Ps...>) TO(reduce<normalize_t<mul<Ps...>>>);

  template <typename ...Ps>
  requires ts::packs::Contains<no_unit, Ps...>
  REDUCE_PATTERN(mul<Ps...>) TO(reduce<cancel_out<mul<Ps...>, no_unit>>);

}
