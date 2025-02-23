// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async:system_manager;

import std;
import reflect;

export import fabric.async.scheduler;

export namespace fabric::async {
  class system_base_t {
  public:
    virtual ~system_base_t() {}

    virtual void run() = 0;
  };

  struct system_options_t {
    bool enabled = true;
    duration period;
  };

  struct system_instance_t {
    system_options_t options;
    std::shared_ptr<system_base_t> ptr;
    time_point next_exec;
  };

  class system_manager_t: public virtual scheduler_t {
  private TEST_PUBLIC: //
    std::unordered_map<refl::type_id_t, system_instance_t> systems_;

  protected TEST_PUBLIC: /// @name Bus Interface
    template <typename T, typename... Args>
      requires std::derived_from<T, system_base_t>
    void add_system(system_options_t opts, Args&&... args) {
      time_point now = clock::now();
      std::shared_ptr<system_base_t> obj = std::make_shared<T>(std::forward<Args>(args)...);
      systems_[refl::type_id<T>] = {opts, obj, now};
      this->cv.notify_all();
    }

    void run_systems() {
      time_point now = clock::now();
      for (auto& [type_id, sys]: systems_) {
        if (sys.options.enabled and sys.next_exec < now) {
          sys.next_exec = now + sys.options.period;
          this->set_next_wakeup(sys.next_exec);
          sys.ptr->run();
        }
      }
    }

  public: /// @name Public Interface
    template <typename T>
      requires std::derived_from<T, system_base_t>
    T& get_system() {
      static constexpr refl::type_id_t type_id = refl::type_id<T>();
      auto it = systems_.find(type_id);
      if (it == systems_.end()) {
        throw std::runtime_error(std::format("No such system type in bus: {}", refl::type_name<T>));
      }
      return *dynamic_cast<T*>(it->second.ptr.get());
    }

  public: /// @name Getter
    system_manager_t& get_system_manager() {
        return *this;
    }
  };
}
