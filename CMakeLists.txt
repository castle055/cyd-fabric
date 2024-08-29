# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later


################################################################################
#[[                            PROJECT OVERVIEW                              ]]#
################################################################################
#                                                                CYD-FABRIC    #
#                                                                              #
#[[  AVAILABLE OPTIONS:                                                      ]]#
#      - FABRIC_BUILD_TESTS ............................. DEV_MODE only, ON    #
#            whether or not tests should be built                              #
#      - FABRIC_BUILD_DOCS .............................. DEV_MODE only, ON    #
#            whether or not the documentation should be built                  #
#                                                                              #
#[[  CMAKE STRUCTURE:                                                        ]]#
#      - Project setup                                                         #
#      - Configure dependencies:                                               #
#          - STD Module   (system)                                             #
#          - Cairomm      (1.18.0)                                             #
#      - Configure CYD_FABRIC                                                  #
#      - Configure tests                                                       #
#      - Configure Doxygen documentation                                       #
#                                                                              #
################################################################################


################################################################################
#[[                              PROJECT SETUP                               ]]#
################################################################################
#[[                          PREVENT IN-TREE BUILD                           ]]#
if (CMAKE_SOURCE_DIR STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. Create a `build/` directory and call `cmake ..` from there.")
endif ()

#[[                          CHECK DEVELOPER MODE                            ]]#
set(FABRIC_DEV_MODE OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(FABRIC_DEV_MODE ON)
endif ()

#[[                         DEVELOPER MODE OPTIONS                           ]]#
if(FABRIC_DEV_MODE)
    option(FABRIC_BUILD_TESTS "whether or not tests should be built" ON)
    option(FABRIC_BUILD_DOCS "whether or not the documentation should be built" ON)
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
add_library(std)
target_sources(std
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS /usr/share/libc++/v1
        FILES /usr/share/libc++/v1/std.cppm
        /usr/share/libc++/v1/std.compat.cppm
)
target_compile_options(std
    PUBLIC
    -Wno-reserved-module-identifier
    -Wno-deprecated-declarations
)

#[[                          CONFIGURE CAIROMM-1.16                          ]]#
find_package(CAIROMM)


################################################################################
#[[                           CONFIGURE CYD FABRIC                           ]]#
################################################################################
add_library(cyd_fabric)

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
        cairomm
        yaml-cpp jpeg
        unwind
        libstdc++exp.a
        std
)


################################################################################
#[[                             CONFIGURE TESTS                              ]]#
################################################################################
include(${FABRIC_CMAKE_DIR}/ctest.cmake)

if (FABRIC_DEV_MODE AND FABRIC_BUILD_TESTS)
    target_configure_test_directory(cyd_fabric ${FABRIC_TEST_DIR})
endif ()


################################################################################
#[[                     CONFIGURE DOXYGEN DOCUMENTATION                      ]]#
################################################################################
include(${FABRIC_CMAKE_DIR}/docs.cmake)

if (FABRIC_DEV_MODE AND FABRIC_BUILD_DOCS)
    target_configure_doxygen(cyd_fabric
            DOXYFILE ${FABRIC_DIR}/Doxyfile
            BASE_DIR ${FABRIC_DIR}
    )
endif ()

