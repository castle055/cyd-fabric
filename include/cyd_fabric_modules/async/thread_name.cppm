// Copyright (c) 2025, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  thread_name.cppm
 *! \brief
 *!
 */

module;
#ifdef __linux__
#include <pthread.h>
#endif

export module fabric.thread_name;

import std;
export import reflect;

export namespace fabric {
#ifdef __linux__
  std::string get_thread_name() {
    std::string name{};
    name.resize(16);
    pthread_getname_np(pthread_self(), name.data(), name.size());
    name.resize(name.find_first_of('\0'));
    return name;
  }
  void set_thread_name(const std::string& name) {
    pthread_setname_np(pthread_self(), name.c_str());
  }
#endif
} // namespace fabric
