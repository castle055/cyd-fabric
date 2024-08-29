// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  target_base.cppm
 *! \brief 
 *!
 */
export module fabric.logging.target_base;
import std;

export import fabric.logging.entry;

export namespace LOG {
  constexpr const char* DEFAULT_ENTRY_FORMAT {"[{entry:level}] {entry:timestamp} | [{entry:path}:{entry:line}] [{entry:function}] {entry:message}"};

  struct target_id {
    using hash_type = std::size_t;

    std::string id_string;
    hash_type id_hash;

    target_id(const std::string &id): id_string(id), id_hash(std::hash<std::string> { }(id_string)) {
    }
    target_id(std::string &&id): id_string(id), id_hash(std::hash<std::string> { }(id_string)) {
    }
  };

  struct target_base {
    explicit target_base(const std::string &&id_): id(std::forward<const std::string &&>(id_)) {
    }

    virtual ~target_base() noexcept = default;

    virtual void append(const entry_t &entry) = 0;

    const target_id id;
  };

  template<typename T>
  concept TargetBuilder = requires(T t)
  {
    { t.operator()() } -> std::convertible_to<std::unique_ptr<target_base>>;
  };

  template<typename T>
  concept DerivesTarget = requires
  {
    requires std::derived_from<T, target_base>;
  };
}

export namespace LOG::TARGETS {
  /* empty */
};
