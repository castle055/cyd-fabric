# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later

################################################################################
#[[                            PROJECT OVERVIEW                              ]]#
################################################################################
#                                                                              #
#    CMAKE STRUCTURE:                                                          #
#      - Project setup                                                         #
#      - Configure dependencies:                                               #
#          - STD Module                                                        #
#          - Cairomm-1.16                                                      #
#      - Configure CYD_FABRIC                                                  #
#      - Configure tests                                                       #
#      - Configure Doxygen documentation                                       #
#                                                                              #
################################################################################

################################################################################
#[[                              PROJECT SETUP                               ]]#
################################################################################
if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. Create a `build/` directory and call `cmake ..` from there.")
endif ()

# Select 'Release' build type by default.
# Has to be done before the call to `project()`.
# Use `-DCMAKE_BUILD_TYPE=` to override this.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")

cmake_minimum_required(VERSION 3.29.20240518 FATAL_ERROR)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_fabric LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -std=c++23")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

message("CMAKE VERSION: " ${CMAKE_VERSION})
message("COMPILER: " ${CMAKE_CXX_COMPILER})

SET(FABRIC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(FABRIC_TEST_DIR ${FABRIC_DIR}/test)
SET(FABRIC_INCLUDE_DIR ${FABRIC_DIR}/include)
SET(FABRIC_SOURCE_DIR ${FABRIC_DIR}/src)
SET(FABRIC_CMAKE_DIR ${FABRIC_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${FABRIC_CMAKE_DIR}/Modules)


include(cmake/compiler_flags.cmake)
compiler_flags(
        "-stdlib=libc++"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"

#        "-Wliteral-suffix"

        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        "-Wno-error=unused"
        "-Wno-unused-local-typedefs"

        "-Wno-unknown-pragmas"
#        "-freport-bug"
)

################################################################################
#[[                          CONFIGURE DEPENDENCIES                          ]]#
################################################################################
include(ExternalProject)
include(FetchContent)
find_package(PkgConfig)

#[[                           CONFIGURE STD MODULE                           ]]#
FetchContent_Declare(
  std
  URL "file:///usr/lib/c++/v1/"
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SYSTEM
)
FetchContent_MakeAvailable(std)

#[[                          CONFIGURE CAIROMM-1.16                          ]]#
find_package(CAIROMM)
#pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
#include_directories(${CAIROMM-1.16_INCLUDE_DIRS})


################################################################################
#[[                           CONFIGURE CYD FABRIC                           ]]#
################################################################################

FILE(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        ${FABRIC_INCLUDE_DIR}/*.h
        ${FABRIC_INCLUDE_DIR}/*.hpp
        ${FABRIC_INCLUDE_DIR}/*.cpp
        ${FABRIC_SOURCE_DIR}/*.h
        ${FABRIC_SOURCE_DIR}/*.hpp
        ${FABRIC_SOURCE_DIR}/*.cpp
)
FILE(GLOB_RECURSE SRC_MOD_LIST CONFIGURE_DEPENDS
        ${FABRIC_INCLUDE_DIR}/*.cppm
        ${FABRIC_SOURCE_DIR}/*.cppm
)

add_library(cyd_fabric)
target_include_directories(cyd_fabric PUBLIC ${FABRIC_INCLUDE_DIR})
target_compile_features(cyd_fabric PUBLIC cxx_std_23)
target_sources(cyd_fabric
        PUBLIC ${SRC_LIST}
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS ${FABRIC_SOURCE_DIR} ${FABRIC_INCLUDE_DIR}
        FILES ${SRC_MOD_LIST}
)
target_link_libraries(cyd_fabric
        PUBLIC
        X11 Xft Xrender fontconfig
#        ${CAIROMM-1.16_LIBRARIES}
        cairomm
        yaml-cpp jpeg
        libstdc++exp.a
        std
)

################################################################################
#[[                             CONFIGURE TESTS                              ]]#
################################################################################
include(${FABRIC_CMAKE_DIR}/ctest.cmake)
target_configure_test_directory(cyd_fabric ${FABRIC_TEST_DIR})

################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
include(${FABRIC_CMAKE_DIR}/docs.cmake)
target_configure_doxygen(cyd_fabric
        DOXYFILE ${FABRIC_DIR}/Doxyfile
        BASE_DIR ${FABRIC_DIR}
)

