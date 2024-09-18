// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later


#include <cassert>
#include "common/common.h"


import fabric.ts.apply;
import fabric.ts.packs;
import fabric.wiring.signals;


using namespace fabric::wiring;
using namespace fabric::ts;

struct Button {
  private_signal<Button, int> click_signal{};

  slot<void(int)>             click_happened{[&](int b) { click_signal.emit(b); }};
};

struct SomeUI: auto_disconnect {
  Button b1{};
  Button b2{};

  SomeUI() {
    b1.click_signal.connect(control_slot);
  }


  slot<void(int)> control_slot{this, &SomeUI::some_hook};
  void            some_hook(int a) {
    some_signal.emit(2);
  }

  private_signal<SomeUI, int> some_signal{};
};

void setup() {}

TEST("Initial prototyping") {
  signal<int> button_one{};
  signal<int> button_two{};

  SomeUI      ui{};
  button_one.connect(ui.b1.click_happened);
  button_two.connect(ui.b2.click_happened);

  ui.some_signal.connect([](int a) {});

  // ui = {};

  return 0;
}


template <typename R, typename... Args>
std::function<R(Args...)> as_parametrized(std::function<R(std::tuple<Args...>)> func) {
  return [&](Args... args) -> R { return func(std::tuple<Args...>{args...}); };
}
template <typename... Args>
std::function<void(Args...)> as_parametrized(std::function<void(std::tuple<Args...>)> func) {
  return [=](Args... args) -> void { return func(std::tuple<Args...>{args...}); };
}

template <typename...>
struct append_all;

template <typename Pack>
struct append_all<Pack> {
  using type = Pack;
};
template <typename First, typename... Packs>
struct append_all<First, Packs...> {
private:
  using recurse = typename append_all<Packs...>::type;

public:
  using type =
    typename with_type<First>::template apply_as_pack<packs::append<recurse>::template to>::done;
};

template <typename DestTuple, typename SrcTuple, std::size_t OFFSET>
struct copy_tuple {
public:
  static void operator()(DestTuple dest, SrcTuple src) {
    impl_(dest, src, std::make_index_sequence<packs::get_size<SrcTuple>::value>{});
  }

private:
  template <std::size_t... I>
  static void impl_(DestTuple dest, SrcTuple src, std::index_sequence<I...>) {
    ((std::get<(OFFSET) + (I)>(dest) = std::get<I>(src)), ...);
  }
};


template <std::size_t, typename...>
struct cumulative_sizes_impl;

template <std::size_t Accumulator>
struct cumulative_sizes_impl<Accumulator> {
  using type = packs::integer_pack<Accumulator>;
};

template <std::size_t Accumulator, typename Pack>
struct cumulative_sizes_impl<Accumulator, Pack> {
  using type = packs::integer_pack<Accumulator>;
};

template <std::size_t Accumulator, typename First, typename... Packs>
struct cumulative_sizes_impl<Accumulator, First, Packs...> {
private:
  template <std::size_t NewAccumulator>
  using recurse = typename cumulative_sizes_impl<NewAccumulator, Packs...>::type;

public:
  using type = typename with_type<packs::integer_pack<Accumulator>>::template apply_as_pack<
    packs::append<recurse<Accumulator + packs::get_size<First>::value>>::template to>::done;
};

template <typename... Packs>
using cumulative_sizes = typename cumulative_sizes_impl<0, Packs...>::type;

template <template <SignalConcept...> typename Strategy, SignalConcept... S>
  requires StrategyConcept<Strategy>
class signal_block {
public:
  using strategy_t   = Strategy<S...>;
  using state_t      = typename strategy_t::state_type;
  using out_signal_t = typename strategy_t::output_signal_type;


private:
  template <SignalConcept Signal, std::size_t ID>
  static auto connect_signal(state_t& state, Signal& signal, out_signal_t& output_signal) {
    std::function<void(signal_args<Signal>)> func = [&](signal_args<Signal> args) {
      strategy_t::template on_signal<Signal, ID>(state, args);
      strategy_t::maybe_emit(state, output_signal);
    };
    return signal.connect(as_parametrized(func));
  }

  template <SignalConcept... S_>
  struct connect_signals {
  private:
    using signal_ids = std::make_index_sequence<sizeof...(S_)>;
    template <std::size_t... IDs>
    static void impl_(
      state_t&                 state,
      std::vector<connection>& connections,
      out_signal_t&            output_signal,
      std::index_sequence<IDs...>,
      S_&... signal
    ) {
      ((connections.push_back(connect_signal<S_, IDs>(state, signal, output_signal))), ...);
    }

  public:
    static void operator()(
      state_t&                 state,
      std::vector<connection>& connections,
      out_signal_t&            output_signal,
      S_&... signals
    ) {
      impl_(state, connections, output_signal, signal_ids{}, signals...);
    }
  };

public:
  explicit signal_block(S&... s) {
    connections_.reserve(sizeof...(S));
    connect_signals<S...>::operator()(state_, connections_, output, s...);
  }
  ~signal_block() {
    for (auto& connection: connections_) {
      connection.disconnect();
    }
  }

  out_signal_t output;

private:
  std::vector<connection> connections_{};
  state_t                 state_{};
};

template <SignalConcept... Signals>
struct synchronizer_strategy {
  using output_signal_args = typename append_all<std::tuple<>, Signals...>::type;
  using output_signal_type =
    typename append_all<private_signal<synchronizer_strategy>, output_signal_args>::type;
  using signal_offsets = cumulative_sizes<Signals...>;

  struct state_type {
    output_signal_args                   parameters_{};
    std::array<bool, sizeof...(Signals)> signal_received_{};
  };

  template <SignalConcept Signal, std::size_t ID>
  static void on_signal(state_type& state, signal_args<Signal>& args) {
    static constexpr std::size_t signal_offset = packs::get<ID, signal_offsets>::value;
    copy_tuple<output_signal_args, signal_args<Signal>, signal_offset>::operator()(
      state.parameters_, args
    );
    state.signal_received_[ID] = true;
  }

  static void maybe_emit(state_type& state, output_signal_type& output_signal) {
    for (const auto& signal_received: state.signal_received_) {
      if (!signal_received) {
        return;
      }
    }

    output_signal.emit(state.parameters_);

    for (auto& signal_received: state.signal_received_) {
      signal_received = false;
    }
  }
};

template <SignalConcept... Signals>
class synchronizer: public signal_block<synchronizer_strategy, Signals...> {
public:
  explicit synchronizer(Signals&... s)
      : signal_block<synchronizer_strategy, Signals...>(s...) {}
};

template <typename... Signals>
synchronizer(Signals&...) -> synchronizer<Signals...>;

#include <sigc++-3.0/sigc++/sigc++.h>

TEST("Synchronizer") {
  signal<int>         one{};
  signal<std::string> two{};
  // signal<int> two{};

  synchronizer        s{one, two};

  connection          c = s.output.connect([](int a, const std::string& b) {});

  struct asdfasdfaa {
    slot<void(int)>         sss;
    connection              cc = c;
    sigc::connection        ccc;
    signal<int>             s{};
    sigc::signal<void(int)> asdfasdf{};
    sigc::slot<void(int)>   asdfasdfs{};
  };

  return 0;
}
