// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ring_buffer.cppm
 *! \brief 
 *!
 */
module;
#include <type_traits>

export module ring_buffer;

export namespace cyd::fabric {
  template<typename EntrySpec>
  concept DetailedEntrySpec = requires
  {
    typename EntrySpec::value_type;
  };

  namespace impl {
    template<typename ValueType>
    class ring_buffer {
    public:
      using entry_t = ValueType;
    };
  }

  template<typename>
  class ring_buffer {
  };

  template<typename EntrySpec>
    requires (!DetailedEntrySpec<EntrySpec>)
  class ring_buffer<EntrySpec>: public impl::ring_buffer<EntrySpec> {
  };

  template<DetailedEntrySpec EntrySpec>
  class ring_buffer<EntrySpec>: public impl::ring_buffer<typename EntrySpec::value_type> {
  };

  // void sdfasdf() {
  //   using testtype1 = ring_buffer<int>::entry_t;
  //   using testtype2 = ring_buffer<struct {
  //     using value_type = double;
  //   }>::entry_t;
  //   struct asdf {
  //     using value_type = double;
  //   };
  //   using testtype3 = ring_buffer<asdf>::entry_t;
  //   using testtype4 = ring_buffer<decltype(std::declval<struct {
  //     using value_type = double;
  //   }>())>::entry_t;
  //
  //   static_assert(DetailedEntrySpec<asdf>);
  //   asdf::value_type asdfd;
  //
  //   constexpr auto a = sizeof(ring_buffer<int>);
  //   constexpr auto b = sizeof(ring_buffer<
  //     struct {
  //       int c;
  //       float f;
  //     }>);
  // }
}
