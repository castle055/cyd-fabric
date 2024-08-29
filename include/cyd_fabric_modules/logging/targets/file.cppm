// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  file.cppm
 *! \brief 
 *!
 */

export module fabric.logging.targets.file;
import fabric.logging.target_base;

export namespace LOG::TARGETS {
  class FILE final: public target_base {
    FILE(
      std::string id,
      std::filesystem::path path,
      bool append,
      bool create_on_demand,
      entry_format_t entry_format
    ): target_base(std::move(id)),
       path_(path),
       append_mode_(append),
       create_on_demand_(create_on_demand),
       entry_format_(std::move(entry_format))
    {
      if (!create_on_demand_) {
        create_file_if_needed(path_, append_mode_);
      }
    }

  public:
    ~FILE() noexcept override {
      log_file.flush();
      log_file.close();
    }

    void append(const entry_t &entry) override {
      if (create_on_demand_ && !log_file.is_open()) {
        create_file_if_needed(path_, append_mode_);
      }
      log_file << entry_format_.format(entry) << std::endl;
    }

  private:
    const std::filesystem::path path_;
    const bool append_mode_ = true;
    const bool create_on_demand_ = false;
    entry_format_t entry_format_ {DEFAULT_ENTRY_FORMAT};

    std::ofstream log_file;

  private:
    void create_file_if_needed(const std::filesystem::path &path, bool app) {
      const auto log_dir = path.parent_path();
      if (!exists(log_dir)) {
        std::filesystem::create_directories(log_dir);
      }
      if (!exists(path)) {
        log_file.open(path, std::ios_base::app);
        if (!app) {
          log_file.clear();
        }
      }
    }

  public: //! Builder
    struct BUILDER {
      explicit BUILDER(std::string &&id): id_(std::move(id)) {
      }

      BUILDER &entry_format(std::string &&fmt) {
        entry_format_ = entry_format_t {fmt};
        return *this;
      }

      BUILDER &append(const bool append) {
        append_ = append;
        return *this;
      }

      BUILDER &create_on_demand(const bool create_on_demand) {
        create_on_demand_ = create_on_demand;
        return *this;
      }

      BUILDER &path(const std::filesystem::path path) {
        path_ = path;
        return *this;
      }


      [[nodiscard]] std::unique_ptr<target_base> operator()() const {
        return std::unique_ptr<FILE>(new FILE(
          id_,
          path_,
          append_,
          create_on_demand_,
          entry_format_
        ));
      }

    private:
      std::string id_;
      std::filesystem::path path_;
      bool append_           = true;
      bool create_on_demand_ = false;
      entry_format_t entry_format_ {DEFAULT_ENTRY_FORMAT};
    };
  };
}
