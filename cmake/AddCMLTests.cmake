# AddCMLTests
# --------
#
# This module defines a helper function add_cml_tests() which provides a
# common interface to automatically discover unit tests and set common options.
#
# Arguments:
#
#     NAME: A single value which specifies the name of the test to build. A
#           CMake target called cml_${NAME}_test will be created.
#
#     SOURCES: A list of the test source files.

include_guard()

function(add_cml_tests)
    set(options)
    set(one_value_args NAME)
    set(multi_value_args SOURCES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unkown arguments passed to add_cml_tests: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if (NOT DEFINED ARG_NAME)
        message(FATAL_ERROR "Attempted to specify a CML test with no name")
    endif()

    if (NOT CML_BUILD_UNIT_TESTS)
        return()
    endif()

    set(TEST_NAME cml_${ARG_NAME}_test)
    add_executable(${TEST_NAME} ${ARG_SOURCES})

    target_link_libraries(${TEST_NAME}
        PRIVATE
            cml::cml_shared
            Trick::Trick
            JEOD::JEOD
            GTest::gtest_main
            GTest::gmock_main
    )

    target_compile_features(${TEST_NAME}
        PRIVATE
            cxx_std_17
    )

    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME} DISCOVERY_TIMEOUT 300)
endfunction()
