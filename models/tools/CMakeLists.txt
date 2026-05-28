include(AddCMLLibrary)
add_cml_library(
    NAME
        tools

    HEADERS
        unit_test/models/data/include/unit_test_dd.h
        unit_test/models/include/unit_test.hh
        unit_test/models/include/unit_test_proto.h

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
