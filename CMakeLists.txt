cmake_minimum_required(VERSION 3.27)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(cyd_fabric)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_SCAN_FOR_MODULES YES)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")

SET(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)
SET(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
SET(SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

include(cmake/compiler_flags.cmake)
compiler_flags(
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"

        "-Wliteral-suffix"

        "-Wno-macro-redefine"
        "-Wno-unused-function"
        "-Wno-unused-parameter"
        "-Wno-unused-variable"
        "-Wno-unused-value"
        "-Wno-unused-but-set-variable"
        "-Wno-missing-field-initializers"
        #"-Wno-error=unused"

        "-Wno-unknown-pragmas"
)


include_directories(${INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIROMM-1.16 REQUIRED cairomm-1.16)
include_directories(${CAIROMM-1.16_INCLUDE_DIRS})


FILE(GLOB_RECURSE SRC_LIST
        ${INCLUDE_DIR}/*.h
        ${INCLUDE_DIR}/*.hpp
        ${INCLUDE_DIR}/*.cpp
        ${SOURCE_DIR}/*.h
        ${SOURCE_DIR}/*.hpp
        ${SOURCE_DIR}/*.cpp
)

add_library(cyd_fabric)
target_sources(cyd_fabric PUBLIC ${SRC_LIST})
target_link_libraries(cyd_fabric
        PRIVATE
        X11 Xft Xrender fontconfig
        ${CAIROMM-1.16_LIBRARIES}
        yaml-cpp jpeg
)

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
