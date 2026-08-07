# This function copies a prebuilt Trick library, such as libtrick.a, to the build
# directory and converts all of the library's internal symbols from strong symbols
# to weak symbols.
#
# This allows us to link against these unit testing versions of Trick libraries to
# get the native Trick functionality that they provide, while also allowing us to
# override (i.e., mock) some of the functions for testing. Mocked functions are
# built into the cml_mocks target below as strong symbols, which will take
# precedence at link-time.
function(create_trick_unit_test_library LIB_PATH)
    # Rename the library from {original}.a to {original}_unit_test.a and place it
    # in the build directory. For example: libtric.a -> libtrick_unit_test.a.
    get_filename_component(LIBNAME ${LIB_PATH} NAME_WE)
    set(LIB_OUTPUT_DIR ${cml_BINARY_DIR}/trick_unit_test_libs)
    set(LIB_OUTPUT_NAME ${LIB_OUTPUT_DIR}/${LIBNAME}_unit_test.a)
    add_custom_command(
        OUTPUT ${LIB_OUTPUT_NAME}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${LIB_OUTPUT_DIR}"
        COMMAND ${CMAKE_OBJCOPY} --weaken ${LIB_PATH} ${LIB_OUTPUT_NAME}
        DEPENDS ${LIB_PATH}
        COMMENT "Generating unit test version of ${LIBNAME}"
        VERBATIM
    )

    # Create a target that creates the unit test version of this Trick library.
    # It will be named the same as the unit test library's name and will depend
    # on the "weakening" process specified above. Ex. libtrick.a's unit test
    # target will be called "libtrick_unit_test".
    set(TARGET_NAME ${LIBNAME}_unit_test)
    set(GENERATE_TARGET ${TARGET_NAME}_generate)
    add_custom_target(${GENERATE_TARGET} DEPENDS ${LIB_OUTPUT_NAME})
    add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
    set_target_properties(${TARGET_NAME} PROPERTIES IMPORTED_LOCATION ${LIB_OUTPUT_NAME})
    add_dependencies(${TARGET_NAME} ${GENERATE_TARGET})
endfunction()

# Create two libraries: one containing mocked CML functionality and one containing
# mocked Trick functionality. CML internal tests will generally always require both,
# but users may pick and choose depending on their project's needs.
if (CML_BUILD_MOCKS)
    if (NOT TARGET GTest::gmock_main)
        # CML's internal build will never hit this case, but users might.
        message(FATAL_ERROR "You're trying to build CML mocks but GMock wasn't found.")
    endif()

    add_library(cml_mocks STATIC)
    add_library(cml::cml_mocks ALIAS cml_mocks)
    target_link_libraries(cml_mocks
        PRIVATE
            GTest::gmock_main
            Trick::Core
            JEOD::JEOD
    )
    target_include_directories(cml_mocks
        PRIVATE
            ${cml_SOURCE_DIR}/..
    )
    target_sources(cml_mocks
        PRIVATE
            mocks/cml/cml_message_mock.cc
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES
                mocks/cml/cml_message_mock.hh
    )

    add_library(cml_trick_mocks STATIC)
    add_library(cml::trick_mocks ALIAS cml_trick_mocks)
    target_link_libraries(cml_trick_mocks
        PRIVATE
            Trick::Core
            GTest::gmock_main
    )
    target_sources(cml_trick_mocks
        PRIVATE
            mocks/trick/exec_proto_mock.cc
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
            FILES
                mocks/trick/exec_proto_mock.hh
    )
endif()

# See the documentation for the create_trick_unit_test_library above. Here, we
# copy Trick's prebuilt libraries into our local build directory, but with all
# the symbols converted to weak symbols. That way, our mocked Trick functionality
# from the cml_trick_mocks target takes precedence at link time.
if (CML_BUILD_UNIT_TESTS)
    create_trick_unit_test_library(${Trick_LIBRARY})
    create_trick_unit_test_library(${Trick_liber7_utils})
    create_trick_unit_test_library(${Trick_libtrick_comm})
    create_trick_unit_test_library(${Trick_libtrick_math})
    create_trick_unit_test_library(${Trick_libtrick_pyip})
    create_trick_unit_test_library(${Trick_libtrick_var_binary_parser})
    create_trick_unit_test_library(${Trick_libtrick_connection_handlers})
    create_trick_unit_test_library(${Trick_libtrick_mm})
    create_trick_unit_test_library(${Trick_libtrick_units})

    # We need to specify all the dependencies for Trick just like we do when
    # finding the real versions of each Trick library.
    add_library(cml::trick_unit_testing_libs ALIAS libtrick_unit_test)
    target_link_libraries(libtrick_unit_test
        INTERFACE
            dl
            pthread
            udunits2
            liber7_utils_unit_test
            libtrick_comm_unit_test
            libtrick_math_unit_test
            libtrick_pyip_unit_test
            libtrick_var_binary_parser_unit_test
            libtrick_connection_handlers_unit_test
            libtrick_mm_unit_test
            libtrick_units_unit_test
    )
    target_link_libraries(libtrick_mm_unit_test INTERFACE libtrick_unit_test)
    target_link_libraries(libtrick_pyip_unit_test INTERFACE libtrick_unit_test)
endif()
