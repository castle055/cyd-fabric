// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  ebus_runner.cppm
 *! \brief 
 *!
 */

export module fabric.async.ebus:runner;

import std;

import fabric.logging;

import :types;
import :impl;

export namespace fabric::async {
  class ebus_runner {
  public:
    explicit ebus_runner(const std::optional<ebus::wptr> &ebus)
      : ebus_(ebus) {
      if (ebus_.has_value() && (not ebus_.value().expired()) && ebus_.value().lock() != nullptr) {
        ebus_.value().lock()->runner_claimed_ = this;
      }
    }

    explicit ebus_runner(std::optional<ebus::wptr>&& ebus)
      : ebus_(ebus) {
      if (ebus_.has_value() && (not ebus_.value().expired()) && ebus_.value().lock() != nullptr) {
        ebus_.value().lock()->runner_claimed_ = this;
      }
    }

    ~ebus_runner() {
      if (operator bool()) {
        ebus_.value().lock()->runner_claimed_ = nullptr;
      }
    }

    ebus_runner(const ebus_runner &) = delete;
    ebus_runner &operator=(const ebus_runner &) = delete;
    ebus_runner(ebus_runner && rhl) noexcept: ebus_(std::move(rhl.ebus_)) {
      rhl.ebus_ = std::nullopt;
      if (ebus_.has_value() && (not ebus_.value().expired()) && ebus_.value().lock() != nullptr) {
        ebus_.value().lock()->runner_claimed_ = this;
      }
    }
    ebus_runner &operator=(ebus_runner && rhl) noexcept {
      this->ebus_ = std::move(rhl.ebus_);
      rhl.ebus_ = std::nullopt;
      if (ebus_.has_value() && (not ebus_.value().expired()) && ebus_.value().lock() != nullptr) {
        ebus_.value().lock()->runner_claimed_ = this;
      }
      return *this;
    };

    void events_process_batch() const {
      if (operator bool()) {
        ebus_.value().lock()->events_process_batch();
      } else {
        LOG::print {ERROR}("ebus runner is invalid or was evicted");
      }
    }

    operator bool() const {
      return ebus_.has_value()
             && (not ebus_.value().expired())
             && (ebus_.value().lock() != nullptr)
             && (ebus_.value().lock()->runner_claimed_ == this);
    }

  private:
    std::optional<ebus::wptr> ebus_;
  };


  ebus_runner ebus::sptr::claim_runner() const {
    if ((*this)->is_runner_claimed()) {
      LOG::print {WARN}("instantiated an invalid ebus runner");
      return ebus_runner {std::nullopt};
    } else {
      return ebus_runner {*this};
    }
  }
}