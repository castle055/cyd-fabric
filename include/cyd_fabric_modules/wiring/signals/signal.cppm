// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  signal.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:signal;

import std;

import fabric.ts.apply;
import fabric.ts.packs;

export import :types;
export import :ts;
export import :slot;
export import :connection;
export import :auto_disconnect;

export import reflect;


template <typename... Args>
class fabric::wiring::signal: auto_disconnect {
public:
  friend class connection;
  using slot_type       = slot<void(Args...)>;
  using args_tuple_type = typename ts::with_type<std::tuple<>>::apply_as_pack<
    ts::packs::append<signal>::template to>::done;


  signal()                            = default;
  ~signal()                           = default;

  signal(const signal&)               = delete;
  signal(signal&&)                    = delete;
  signal&    operator=(const signal&) = delete;
  signal&    operator=(signal&&)      = delete;


  connection connect(const slot<void(Args...)>& slot) {
    auto id = next_id_++;
    connected_slots_.emplace_front(id, slot);
    auto c = connection{this, id};
    if (slot.lifetime_bound()) {
      slot.get_auto_disconnect_object()->add_connection(c);
    }
    return c;
  }

  connection connect(slot<void(Args...)>&& slot) {
    auto id = next_id_++;
    connected_slots_.emplace_front(id, slot);
    auto c = connection{this, id};
    if (slot.lifetime_bound()) {
      slot.get_auto_disconnect_object()->add_connection(c);
    }
    return c;
  }

  void emit(Args... args) const {
    for (const auto& [id, connected_slot]: connected_slots_) {
      connected_slot(args...);
    }
  }


  template <template <typename...> typename Pack>
  void emit(Pack<Args...> args) const {
    [&]<std::size_t... I>(std::index_sequence<I...>) {
      emit(std::get<I>(args)...);
    }(std::make_index_sequence<sizeof...(Args)>{});
  }


private:
  bool disconnect(std::uint64_t slot_id) {
    auto prev = connected_slots_.before_begin();
    for (auto it = connected_slots_.begin(); it != connected_slots_.end(); ++it) {
      auto& [id, _] = *it;
      if (slot_id == id) {
        connected_slots_.erase_after(prev);
        return true;
      }
      prev = it;
    }
    return false;
  }

private:
  std::forward_list<std::pair<slot_id_t, slot<void(Args...)>>> connected_slots_{};
  slot_id_t                                                    next_id_ = 0;

  char padding_[4];
};


// static fabric::refl::internal::module_registry __module_registry{
//   {.records = {{
//      .type = fabric::refl::type_id::from<fabric::wiring::signal<int>>(),
//    }}}
// };

