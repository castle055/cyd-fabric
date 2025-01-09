// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  signal.cppm
 *! \brief
 *!
 */

export module fabric.wiring.signals:signal;

import std;

import fabric.ts.apply;
import packtl;

export import :types;
export import :ts;
export import :slot;
export import :connection;
export import :auto_disconnect;

export import reflect;

template <typename... Args>
struct signal_data {
  std::forward_list<std::pair<fabric::wiring::slot_id_t, fabric::wiring::slot<void(Args...)>>>
                            connected_slots_{};
  fabric::wiring::slot_id_t next_id_ = 0;
};

export template <typename... Args>
class fabric::wiring::signal: auto_disconnect {
public:
  friend class connection;
  using slot_type       = slot<void(Args...)>;
  using args_tuple_type = typename ts::with_type<std::tuple<>>::apply_as_pack<
    packtl::append<signal>::template to>::done;


  signal()  = default;
  ~signal() = default;

  signal(const signal&)            = default;
  signal(signal&&)                 = default;
  signal& operator=(const signal&) = default;
  signal& operator=(signal&&)      = default;


  connection connect(const slot<void(Args...)>& slot) {
    auto id = signal_data_->next_id_++;
    signal_data_->connected_slots_.emplace_front(id, slot);
    auto c = connection{this, id};
    if (slot.lifetime_bound()) {
      slot.get_auto_disconnect_object()->add_connection(c);
    }
    return c;
  }

  connection connect(slot<void(Args...)>&& slot) {
    auto id = signal_data_->next_id_++;
    signal_data_->connected_slots_.emplace_front(id, slot);
    auto c = connection{this, id};
    if (slot.lifetime_bound()) {
      slot.get_auto_disconnect_object()->add_connection(c);
    }
    return c;
  }

  void emit(Args... args) const {
    for (const auto& [id, connected_slot]: signal_data_->connected_slots_) {
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
    auto prev = signal_data_->connected_slots_.before_begin();
    for (auto it = signal_data_->connected_slots_.begin(); it != signal_data_->connected_slots_.end(); ++it) {
      auto& [id, _] = *it;
      if (slot_id == id) {
        signal_data_->connected_slots_.erase_after(prev);
        return true;
      }
      prev = it;
    }
    return false;
  }

private:
  std::shared_ptr<signal_data<Args...>> signal_data_ = std::make_shared<signal_data<Args...>>();
};

// static fabric::refl::internal::module_registry __module_registry{
//   {.records = {{
//      .type = fabric::refl::type_id::from<fabric::wiring::signal<int>>(),
//    }}}
// };

