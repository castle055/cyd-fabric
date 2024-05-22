cmake_minimum_required(VERSION 3.29.20240518 FATAL_ERROR)
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
#set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cyd_fabric LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts -std=c++23")

message("CMAKE VERSION: " ${CMAKE_VERSION})
message("COMPILER: " ${CMAKE_CXX_COMPILER})

SET(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)
SET(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
SET(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

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

include(FetchContent)
FetchContent_Declare(
  std
  URL "file:///usr/lib/c++/v1/"
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SYSTEM
)
FetchContent_MakeAvailable(std)

include_directories(${INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
include_directories(${CAIROMM-1.16_INCLUDE_DIRS})


FILE(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS
        ${INCLUDE_DIR}/*.h
        ${INCLUDE_DIR}/*.hpp
        ${INCLUDE_DIR}/*.cpp
        ${SOURCE_DIR}/*.h
        ${SOURCE_DIR}/*.hpp
        ${SOURCE_DIR}/*.cpp
)
FILE(GLOB_RECURSE SRC_MOD_LIST CONFIGURE_DEPENDS
        ${INCLUDE_DIR}/*.cppm
        ${SOURCE_DIR}/*.cppm
)

#message(${CMAKE_CXX_COMPILER})
#message(FLAGS: ${CMAKE_CXX_FLAGS})
#add_custom_target(std_modules
#        COMMAND ${CMAKE_CXX_COMPILER} -fmodules-ts -std=gnu++23 -fno-module-lazy -g -c -xc++-user-header string
#)
#include_directories(SYSTEM build/debug/gcm.cache)

add_library(cyd_fabric)
target_compile_features(cyd_fabric PUBLIC cxx_std_23)
add_dependencies(cyd_fabric std)
target_sources(cyd_fabric
        PUBLIC ${SRC_LIST}
        PUBLIC
        FILE_SET cxx_modules
        #        FILE_SET "${APP_NAME}_modfiles"
        TYPE CXX_MODULES
        BASE_DIRS ${SOURCE_DIR} ${INCLUDE_DIR}
        FILES ${SRC_MOD_LIST}
)
target_link_libraries(cyd_fabric
        PUBLIC
        X11 Xft Xrender fontconfig
        ${CAIROMM-1.16_LIBRARIES}
        yaml-cpp jpeg
        libstdc++exp.a
        std
)
include_directories(cyd_fabric PUBLIC ${CAIROMM-1.16_INCLUDE_DIRS})

# Build Tests
include(cmake/ctest.cmake)

# Build Doxygen Documentation
include(cmake/docs.cmake)

# Build Local tests
if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/local_test)
    FILE(GLOB_RECURSE TEST_LIST
            ${CMAKE_CURRENT_LIST_DIR}/local_test/*.h
            ${CMAKE_CURRENT_LIST_DIR}/local_test/*.hpp
            ${CMAKE_CURRENT_LIST_DIR}/local_test/*.cpp
            #        ${TEST_DIR}/**/*.*
    )
    #FILE(GLOB_RECURSE TEST_MOD_LIST
    #        ${TEST_DIR}/*.cppm
    #        ${TEST_DIR}/**/*.*
    #)
    add_executable(cyd_fabric_test ${TEST_LIST})
    #target_sources(cyd_fabric_test
    #        PRIVATE FILE_SET CXX_MODULES FILES ${TEST_MOD_LIST})
    target_include_directories(cyd_fabric_test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/local_test)
    target_link_libraries(cyd_fabric_test
            PRIVATE cyd_fabric
    )
endif ()

if (CMAKE_BUILD_TYPE MATCHES "Release")
    add_custom_command(TARGET cyd_fabric POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory dist
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )
    add_custom_command(TARGET cyd_fabric POST_BUILD
            COMMAND cp -v
            $<TARGET_FILE:cyd_fabric>
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying binary into distribution directory..."
    )
    add_custom_command(TARGET cyd_fabric POST_BUILD
            COMMAND cp -rv
            ${INCLUDE_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/dist/
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            COMMENT "Copying header files into distribution directory..."
    )
endif ()

