include(AddCMLLibrary)
add_cml_library(
    NAME
        tools

    HEADERS
        unit_test/models/include/unit_test.hh

    SOURCES
        unit_test/models/src/unit_test.cc
)

include(AddCMLTests)
add_cml_tests(
    NAME
        tools

    SOURCES
        unit_test/models/tests/test_unit_framework.cc
)
