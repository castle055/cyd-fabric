# Copyright (c) 2024, Víctor Castillo Agüero.
# SPDX-License-Identifier: GPL-3.0-or-later

enable_testing()

function(target_configure_test_directory TEST_TARGET TEST_DIR)
    get_filename_component(TEST_DIR ${TEST_DIR} REALPATH)

    FILE(GLOB_RECURSE TEST_LIST
            ${TEST_DIR}/**/*.cpp
            ${TEST_DIR}/*.cpp
    )
    foreach (test ${TEST_LIST})
        set(TDir ${test})
        get_filename_component(TDir ${TDir} REALPATH)
        get_filename_component(TDir ${TDir} DIRECTORY)
        string(REPLACE ${TEST_DIR} "" TDir ${TDir})
        if (NOT "${TDir}" STREQUAL "")
            string(SUBSTRING ${TDir} 1 -1 TDir)
        endif ()

        get_filename_component(TName ${test} NAME_WLE)
        add_executable(TEST_${TName} ${test})
        target_link_libraries(TEST_${TName} PRIVATE ${TEST_TARGET})
        target_include_directories(TEST_${TName} PRIVATE ${TEST_DIR}/common)

        set(TFullName ${TName})
        if (NOT "${TDir}" STREQUAL "")
            set(TFullName "${TDir}/${TName}")
        endif ()

        file(STRINGS ${test} TLines)
        foreach (line ${TLines})
            if ("${line}" MATCHES "^TEST\\(\"(.*)\"\\)")
                set(case ${CMAKE_MATCH_1})
                add_test(NAME "${TFullName} - ${case}" COMMAND $<TARGET_FILE:TEST_${TName}> "${case}")
            endif ()
        endforeach ()
    endforeach ()
endfunction()