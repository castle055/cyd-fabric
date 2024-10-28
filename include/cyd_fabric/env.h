// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CYD_UI_ENV_H
#define CYD_UI_ENV_H

#include <cstdlib>
#include <string>

/**
 * \brief Process environmental variables interface
 *
 * This namespace provides an abstraction over environmental variables.
 */
namespace cyd::fabric::env {
  /**
   * \brief Get the value of the environmental variable with name `ENVVAR`
   * \returns `std::string` containing the value of the variable
   */
  inline std::string get(const std::string &ENVVAR /**< [in] Variable name */) {
    const char* s = std::getenv(ENVVAR.c_str());
    return s == nullptr? "": std::string{s};
  }

  /**
   * \brief Set the value of the environmental variable with name `ENVVAR` to `VALUE`
   * \note This function **overwrites** the old value of the variable
   * \returns `std::string` containing the **old value** of the variable
   */
  inline std::string set(
    const std::string &ENVVAR,
    ///< [in] Variable name
    const std::string &VALUE ///< [in] New value
  ) {
    std::string prev = get(ENVVAR);
    setenv(ENVVAR.c_str(), VALUE.c_str(), true);
    return prev;
  }

  struct var {
    const std::string &ENVVAR;

    operator std::string() {
      return env::get(ENVVAR);
    }

    var &operator=(const std::string &VALUE) {
      env::set(ENVVAR, VALUE);
      return *this;
    }
  };

#define ENV(NAME, ENV, ...)                 \
  inline std::string NAME() {               \
    auto s = cyd::fabric::env::get(#ENV);   \
    return s.empty()                        \
            ? ( __VA_ARGS__ )               \
            : std::string{s};               \
  }

  //! Common Environment Variables

  ENV(USER, USER, "")
  ENV(PWD, PWD, ".")
  ENV(HOME, HOME, std::string{"/home/"} + USER())
}


#endif //CYD_UI_ENV_H
