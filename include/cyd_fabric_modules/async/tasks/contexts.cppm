// Copyright (c) 2024-2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

export module fabric.tasks:contexts;

import std;
import reflect;

export import :types;

constexpr std::uint64_t fnv1a_partial(std::uint64_t partial, std::string_view s) {
  if (s.length() == 0) {
    return 0;
  }

  partial = (partial ^ s[0]) * 1099511628211u;
  return s.length() == 1 ? partial : fnv1a_partial(partial, s.substr(1));
}

// constexpr FNV1a hash
constexpr std::uint64_t fnv1a_append(std::uint64_t partial, std::string_view s) {
  if (s.length() >= 256) {
    return fnv1a_append(fnv1a_partial(partial, s.substr(0, 256)), s.substr(256));
  } else {
    return fnv1a_partial(partial, s);
  }
}

constexpr std::uint64_t fnv1a(std::string_view s) {
  return fnv1a_append(14695981039346656037u, s);
}

export namespace fabric {
  constexpr std::uint64_t hash(std::string_view s) {
    return fnv1a(s);
  }
}

export namespace fabric::tasks {
  template <typename T>
  class task_resource_id {
  public:
    std::uint64_t id_;

  public:
    constexpr task_resource_id()
        : id_(refl::type_id<T>) {}
    constexpr explicit task_resource_id(std::string_view str)
        : id_(refl::type_id<T>) {
      const std::uint64_t h = hash(str);
      id_                   = (id_ << 1U) ^ h;
    }

    constexpr std::uint64_t get_id() const noexcept {
      return id_;
    }
  };

  class task_resource {
    std::shared_ptr<void>  ptr_;
    const refl::type_info* type_info_;

  public:
    template <typename T>
    explicit task_resource(const std::shared_ptr<T>& ptr)
        : ptr_(ptr),
          type_info_(&refl::type_info::from<T>()) {}

    template <typename T>
    std::shared_ptr<T> get() const {
      if (refl::type_id<T> == type_info_->id()) {
        return std::reinterpret_pointer_cast<T>(ptr_);
      } else {
        throw std::runtime_error(
          std::
            format("Bad resource type. Expected: {}, Requested: {}", type_info_->name(), refl::type_name<T>)
        );
      }
    }
  };

  class task_context {
    std::unordered_map<std::uint64_t, task_resource> resource_map_{};

    task_context()                                     = default;
    task_context(const task_context& other)            = default;
    task_context& operator=(const task_context& other) = default;
    task_context(task_context&& other)                 = default;
    task_context& operator=(task_context&& other)      = default;

  public:
    using sptr = std::shared_ptr<task_context>;

    static sptr make() {
      return std::shared_ptr<task_context>(new task_context());
    }

    static sptr make_copy(const sptr& other) {
      return std::shared_ptr<task_context>(new task_context(*other));
    }

    template <typename T>
    void set_resource(const std::shared_ptr<T>& ptr, task_resource_id<T> id = task_resource_id<T>{}) {
      resource_map_.emplace(id.get_id(), ptr);
    }
    template <typename T>
    std::shared_ptr<T> get_resource(task_resource_id<T> id = task_resource_id<T>{}) const {
      if (resource_map_.contains(id.get_id())) {
        return resource_map_.at(id.get_id()).template get<T>();
      }
      return nullptr;
    }
    template <typename T>
    bool has_resource(task_resource_id<T> id = task_resource_id<T>{}) const {
      return resource_map_.contains(id.get_id());
    }
  };
} // namespace fabric::tasks
