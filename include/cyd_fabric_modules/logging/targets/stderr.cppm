// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  stderr.cppm
 *! \brief 
 *!
 */

export module fabric.logging.targets.stderr;
import fabric.logging.target_base;

export namespace LOG::TARGETS {
  class STDERR final: public target_base {
    entry_format_t entry_format_ {DEFAULT_ENTRY_FORMAT};

    explicit STDERR(
      entry_format_t entry_format
    ): target_base("stderr"), entry_format_(std::move(entry_format)) {
    }

  public:
    ~STDERR() noexcept override = default;

    void append(const entry_t &entry) override {
      std::cerr << entry_format_.format(entry) << std::endl;
    }

  public: //! Builder
    struct BUILDER {
      BUILDER() = default;

      explicit BUILDER(std::string &&format): entry_format_(format) {
      }

      BUILDER &entry_format(std::string &&fmt) {
        entry_format_ = entry_format_t {fmt};
        return *this;
      }

      [[nodiscard]] std::unique_ptr<target_base> operator()() const {
        return std::unique_ptr<STDERR>(new STDERR(entry_format_));
      }

    private:
      entry_format_t entry_format_ {DEFAULT_ENTRY_FORMAT};
    };
  };
}
