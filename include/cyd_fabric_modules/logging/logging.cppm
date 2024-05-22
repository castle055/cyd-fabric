// Copyright (c) 2024, Víctor Castillo Agüero.
// SPDX-License-Identifier: GPL-3.0-or-later

/*! \file  logging.cppm
 *! \brief 
 *!
 */
export module fabric.logging;
export import std;

export import :initializer;
//! Commands
export import :print;

//! SUBMODULES
//!   Types:
export import fabric.logging.levels;

//!   Targets:
export import fabric.logging.targets.stdout;
export import fabric.logging.targets.stderr;
export import fabric.logging.targets.file;
