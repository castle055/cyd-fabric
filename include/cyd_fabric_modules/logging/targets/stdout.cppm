// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  stdout.cppm
 *! \brief 
 *!
 */

export module fabric.logging.targets.stdout;
import fabric.logging.target_base;

export namespace LOG::TARGETS {
  class STDOUT final: public target_base {
    entry_format_t entry_format_ {"{entry:message}"};

    explicit STDOUT(
      entry_format_t entry_format
    ): target_base("stdout"), entry_format_(std::move(entry_format)) {
    }

  public:
    ~STDOUT() noexcept override = default;

    void append(const entry_t &entry) override {
      std::cout << entry_format_.format(entry) << std::endl;
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
        return std::unique_ptr<STDOUT>(new STDOUT(entry_format_));
      }

    private:
      entry_format_t entry_format_ {"{entry:message}"};
    };
  };
}
