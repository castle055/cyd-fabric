# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later

FetchContent_Declare(cairo_lib
        URL https://www.cairographics.org/releases/cairo-1.18.0.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(pixman_lib
        URL https://www.cairographics.org/releases/pixman-0.42.2.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(cairomm_lib
        URL https://www.cairographics.org/releases/cairomm-1.18.0.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(pixman_lib cairo_lib cairomm_lib)

find_program(MESON_EXECUTABLE meson REQUIRED)
find_program(MAKE_EXECUTABLE make REQUIRED)
find_program(NINJA_EXECUTABLE ninja REQUIRED)

ExternalProject_Add(
        Pixman
        SOURCE_DIR ${pixman_lib_SOURCE_DIR}
        BINARY_DIR ${pixman_lib_BINARY_DIR}
        CONFIGURE_COMMAND
        export CXXFLAGS=${CMAKE_CXX_FLAGS} &&
        "CC=${CMAKE_C_COMPILER}"
        "CXX=${CMAKE_CXX_COMPILER}"
        ${pixman_lib_SOURCE_DIR}/configure --prefix ${pixman_lib_BINARY_DIR}
        BUILD_COMMAND ${MAKE_EXECUTABLE}
        INSTALL_COMMAND ${MAKE_EXECUTABLE} install
        BUILD_BYPRODUCTS
        ${pixman_lib_BINARY_DIR}/include/pixman
        "${pixman_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}pixman-1${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

file(MAKE_DIRECTORY ${pixman_lib_BINARY_DIR}/include/pixman)

if (NOT TARGET cairo::pixman)
    add_library(cairo::pixman STATIC IMPORTED)
    add_dependencies(cairo::pixman Pixman)
    set_target_properties(cairo::pixman PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_CONFIGURATIONS "None;Debug;Release;RelWithDebInfo;MinSizeRel"
            IMPORTED_LOCATION "${pixman_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}pixman-1${CMAKE_STATIC_LIBRARY_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES ${pixman_lib_BINARY_DIR}/include/pixman
            INTERFACE_LINK_LIBRARIES "${pixman_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}pixman-1${CMAKE_STATIC_LIBRARY_SUFFIX}"
    )
endif ()

list(APPEND CMAKE_PREFIX_PATH ${pixman_lib_BINARY_DIR})

ExternalProject_Add(
        Cairo-1.16
        SOURCE_DIR ${cairo_lib_SOURCE_DIR}
        BINARY_DIR ${cairo_lib_BINARY_DIR}
        CONFIGURE_COMMAND
        export CXXFLAGS=${CMAKE_CXX_FLAGS} &&
        "CC=${CMAKE_C_COMPILER}"
        "CXX=${CMAKE_CXX_COMPILER}"
        ${MESON_EXECUTABLE} setup
        --prefix ${cairo_lib_BINARY_DIR}
        ${cairo_lib_SOURCE_DIR}
        BUILD_COMMAND ${NINJA_EXECUTABLE} -C ${cairo_lib_BINARY_DIR}
        INSTALL_COMMAND ${NINJA_EXECUTABLE} -C ${cairo_lib_BINARY_DIR} install
        BUILD_BYPRODUCTS
        ${cairo_lib_BINARY_DIR}/include/cairo
        "${cairo_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}cairo${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

file(MAKE_DIRECTORY ${cairo_lib_BINARY_DIR}/include/cairo)

if (NOT TARGET cairo::cairo)
    add_library(cairo::cairo STATIC IMPORTED)
    add_dependencies(cairo::cairo Cairo-1.16)
    set_target_properties(cairo::cairo PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_CONFIGURATIONS "None;Debug;Release;RelWithDebInfo;MinSizeRel"
            IMPORTED_LOCATION "${cairo_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}cairo${CMAKE_SHARED_LIBRARY_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES ${cairo_lib_BINARY_DIR}/include/cairo
            INTERFACE_LINK_LIBRARIES "${cairo_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}cairo${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
endif ()

list(APPEND CMAKE_PREFIX_PATH ${cairo_lib_BINARY_DIR})

ExternalProject_Add(
        Cairomm-1.16
        SOURCE_DIR ${cairomm_lib_SOURCE_DIR}
        BINARY_DIR ${cairomm_lib_BINARY_DIR}
        CONFIGURE_COMMAND
            export CXXFLAGS=${CMAKE_CXX_FLAGS} &&
            "CC=${CMAKE_C_COMPILER}"
            "CXX=${CMAKE_CXX_COMPILER}"
            ${MESON_EXECUTABLE} setup
            --prefix ${cairomm_lib_BINARY_DIR}
            --libdir lib ${cairomm_lib_BINARY_DIR} ${cairomm_lib_SOURCE_DIR}
            && ${MESON_EXECUTABLE} configure -Dbuild-tests=false
            && ${MESON_EXECUTABLE} configure -Dbuild-examples=false
            && ${MESON_EXECUTABLE} configure -Dbuild-documentation=false
        BUILD_COMMAND ${NINJA_EXECUTABLE}
        INSTALL_COMMAND ${NINJA_EXECUTABLE} install
        BUILD_BYPRODUCTS
            ${cairomm_lib_BINARY_DIR}/include/cairomm-1.16
            ${cairomm_lib_BINARY_DIR}/lib/cairomm-1.16/include
            "${cairomm_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}cairomm-1.16${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

file(MAKE_DIRECTORY ${cairomm_lib_BINARY_DIR}/include/cairomm-1.16)
file(MAKE_DIRECTORY ${cairomm_lib_BINARY_DIR}/lib/cairomm-1.16/include)

if (NOT TARGET cairo::cairomm)
    add_library(cairo::cairomm STATIC IMPORTED)
    add_dependencies(cairo::cairomm Cairomm-1.16)
    set_target_properties(cairo::cairomm PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_CONFIGURATIONS "None;Debug;Release;RelWithDebInfo;MinSizeRel"
            IMPORTED_LOCATION "${cairomm_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}cairomm-1.16${CMAKE_SHARED_LIBRARY_SUFFIX}"
            INTERFACE_INCLUDE_DIRECTORIES
                "${cairomm_lib_BINARY_DIR}/include/cairomm-1.16;${cairomm_lib_BINARY_DIR}/lib/cairomm-1.16/include"
            INTERFACE_LINK_LIBRARIES "${cairomm_lib_BINARY_DIR}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}cairomm-1.16${CMAKE_SHARED_LIBRARY_SUFFIX}"
    )
    add_dependencies(cairo::cairomm cairo::cairo)
endif ()

list(APPEND CMAKE_PREFIX_PATH ${cairomm_lib_BINARY_DIR})

pkg_search_module(CAIRO-1.16 cairo REQUIRED IMPORTED_TARGET)
pkg_search_module(CAIROMM-1.16 cairomm-1.16 REQUIRED IMPORTED_TARGET)

add_library(cairomm INTERFACE)
target_link_libraries(cairomm
        INTERFACE
        cairo::pixman
        cairo::cairo
        cairo::cairomm
        PkgConfig::CAIROMM-1.16
)