enable_testing()

FILE(GLOB_RECURSE TEST_LIST
        ${CMAKE_CURRENT_LIST_DIR}/../test/**/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../test/*.cpp
)
foreach (test ${TEST_LIST})
    get_filename_component(TName ${test} NAME_WLE)
    add_executable(TEST_${TName} ${test})
    target_link_libraries(TEST_${TName} PRIVATE cyd_fabric)
    target_include_directories(TEST_${TName} PRIVATE ${TEST_DIR}/common)

    file(STRINGS ${test} TLines)
    foreach (line ${TLines})
        if ("${line}" MATCHES "^TEST\\(\"(.*)\"\\)")
            set(case ${CMAKE_MATCH_1})
            add_test(NAME "${TName} - ${case}" COMMAND $<TARGET_FILE:TEST_${TName}> "${case}")
        endif ()
    endforeach ()
endforeach ()
