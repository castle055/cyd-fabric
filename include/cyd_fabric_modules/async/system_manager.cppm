// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

module;
#include <cyd_fabric_modules/headers/macros/test_enabled.h>

export module fabric.async:system_manager;

import std;

export namespace fabric::async {
  class system_manager_t {
  private TEST_PUBLIC: //
    std::vector<std::function<void()>> system_functions_{};

  protected TEST_PUBLIC: /// @name Bus Interface
    void add_system(const std::function<void()>& function) {
      system_functions_.push_back(function);
    }

    void run_systems() {
      for (auto function : system_functions_) {
        function();
      }
    }

  public: /// @name Public Interface
  public: /// @name Getter
    system_manager_t& get_system_manager() {
        return *this;
    }
  };
}
