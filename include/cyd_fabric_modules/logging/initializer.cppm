// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  initializer.cppm
 *! \brief 
 *!
 */
module;
#define NO_COPY(TYPE) TYPE(const TYPE& rhl) = delete; TYPE& operator=(const TYPE& rhl) = delete
#define NO_MOVE(TYPE) TYPE(TYPE&& rhl) = delete; TYPE& operator=(TYPE&& rhl) = delete

export module fabric.logging:initializer;
import :context;

export namespace LOG {
  struct INIT {
    explicit INIT(const char* initializer_file = __builtin_FILE()) {
    }

    NO_COPY(INIT);

    NO_MOVE(INIT);

    ~INIT() {
      flush_pending_filter_builder();
      config::targets.swap(targets_);
      config::filters.swap(filters_);
    }

  private:
    void flush_pending_filter_builder() {
      if (pending_filter_builder_.has_value()) {
        filters_.push_back(pending_filter_builder_->build());
        pending_filter_builder_.reset();
      }
    }
  public:
    filter_builder_t<INIT>& filter() {
      flush_pending_filter_builder();

      pending_filter_builder_.emplace(*this);
      return pending_filter_builder_.value();
    }

    INIT &filter(filter_t &&filter) { return FILTER(std::forward<filter_t &&>(filter)); }
    INIT &FILTER(filter_t &&filter) {
      flush_pending_filter_builder();
      if (!targets_.contains(filter.target_.id_hash)) {
        throw std::runtime_error {std::format("Target ID \"{}\" does not exist", filter.target_.id_string)};
      }
      filters_.emplace_back(filter);
      return *this;
    }

    INIT &filters(std::initializer_list<filter_t> &&filters) { return FILTERS(std::forward<std::initializer_list<filter_t> &&>(filters)); }
    INIT &FILTERS(std::initializer_list<filter_t> &&filters) {
      flush_pending_filter_builder();
      for (auto &&filter: filters) {
        if (!targets_.contains(filter.target_.id_hash)) {
          throw std::runtime_error {std::format("Target ID \"{}\" does not exists", filter.target_.id_string)};
        }
        filters_.emplace_back(filter);
      }
      return *this;
    }

  private:
    void add_target(std::unique_ptr<target_base> &&target) {
      if (targets_.contains(target->id.id_hash)) {
        std::swap(targets_.at(target->id.id_hash), target);
        // throw std::runtime_error {std::format("Target ID \"{}\" has already been defined", target->id.id_string)};
      } else {
        targets_.emplace(target->id.id_hash, std::move(target));
      }
    }

  public:
    INIT &target(const TargetBuilder auto &builder) { return TARGET(builder.build()); }
    INIT &TARGET(const TargetBuilder auto &builder) {
      // using target_type = std::remove_reference_t<decltype(target)>;
      add_target(std::move(builder()));
      return *this;
    }

    template<TargetBuilder... TBs>
    INIT &targets(TBs... builders) { return TARGETS(builders...); }

    template<TargetBuilder... TBs>
    INIT &TARGETS(TBs... builders) {
      // using target_type = std::remove_reference_t<decltype(target)>;
      (add_target(std::move(builders())), ...);
      return *this;
    }

  private:
    std::unordered_map<target_id::hash_type, std::unique_ptr<target_base>> targets_ = config::default_targets();
    std::vector<filter_t> filters_ { };

    std::optional<filter_builder_t<INIT>> pending_filter_builder_ {std::nullopt};
  };
}
