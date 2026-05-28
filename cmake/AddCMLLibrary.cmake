# AddCMLLibrary
# --------
#
# This module defines a helper function add_cml_library() which provides a
# common interface to automatically set project-wide options to each CML
# component library.
#
# Arguments:
#
#     NAME: A single value which specifies the name of the library to build.
#           A CMake target called cml_${NAME} will be created, as well as an
#           alias target called cml::${NAME}.
#
#     HEADERS: A list of the header files associated with this library.
#
#     SOURCES: A list of the source files associated with this library.

include_guard()

function(add_cml_library)
    set(options)
    set(one_value_args NAME)
    set(multi_value_args HEADERS SOURCES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unkown arguments passed to add_cml_library: ${ARG_UNPARSED_ARGUMENTS}")
    endif()
    if (NOT DEFINED ARG_NAME)
        message(FATAL_ERROR "Attempted to specify a CML library with no name")
    endif()

    # Define the library and give it an alias in the cml namespace.
    set(LIBRARY_NAME cml_${ARG_NAME})
    add_library(${LIBRARY_NAME} STATIC)
    add_library(cml::${ARG_NAME} ALIAS ${LIBRARY_NAME})

    target_sources(${LIBRARY_NAME}
        PRIVATE
            ${ARG_SOURCES}
        PUBLIC
            FILE_SET HEADERS
            BASE_DIRS ${cml_SOURCE_DIR}/..
            FILES ${ARG_HEADERS}
    )

    # Make the Trick and JEOD include directories dependencies of this target
    # and other downstream targets. Instrument the build for code coverage if
    # CML_CODE_COVERAGE is true.
    target_link_libraries(${LIBRARY_NAME}
        PUBLIC
            Trick::Trick
            JEOD::JEOD
        PRIVATE
            $<$<BOOL:${CML_CODE_COVERAGE}>:--coverage>
    )

    target_compile_features(${LIBRARY_NAME}
        PRIVATE
            cxx_std_17
    )

    # Add warning flags. Instrument the build for code coverage if CML_CODE_COVERAGE
    # is true.
    target_compile_options(${LIBRARY_NAME}
        PRIVATE
            -Wall
            -Wextra
            $<$<BOOL:${CML_CODE_COVERAGE}>:--coverage>
    )
    if (CML_WARNINGS_AS_ERRORS)
        # TODO: Nino Tarantino 2/11/26: uncomment this as part of #24.
        #set_target_properties(
        #    ${LIBRARY_NAME}
        #    PROPERTIES COMPILE_WARNING_AS_ERROR ON
        #)
    endif()

    set_target_properties(
        ${LIBRARY_NAME}
        PROPERTIES POSITION_INDEPENDENT_CODE ON
    )
endfunction()
