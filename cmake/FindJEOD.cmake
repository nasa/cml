# FindJEOD
# --------
#
# This module looks for an installation of JEOD. The following variables are
# populated for external use:
#
#     JEOD_HOME : the JEOD root directory
#
# Additionally, the following target is created that specifies JEOD include
# paths and libraries to link against:
#
#     JEOD::JEOD
#
# When using find_package(JEOD COMPONENTS ...), the following components are
# possible:
#
#     Core    : The core JEOD repository
#     Library : A prebuilt libjeod
#
# We search the following locations for JEOD, in order:
#     1. Using the JEOD_HOME environment variable
#     2. Looking one level up from the CML root directory
#     3. Looking in the externals/ directory

set(JEOD_Core_FOUND FALSE)
set(JEOD_Library_FOUND FALSE)

add_library(jeod INTERFACE)
add_library(JEOD::JEOD ALIAS jeod)

find_path(JEOD_HOME
    NAMES README_jeod
    HINTS
        ENV JEOD_HOME
        ${cml_SOURCE_DIR}/../jeod
        ${cml_SOURCE_DIR}/externals/jeod
    NO_SYSTEM_ENVIRONMENT_PATH
)

if (JEOD_HOME)
    set(JEOD_Core_FOUND TRUE)
    find_library(Trick QUIET)

    # Add include paths for anything that depends on JEOD, and mark those as
    # system paths so we don't get any compiler or linter warnings from them.
    target_include_directories(jeod SYSTEM
        INTERFACE
            ${JEOD_HOME}/models

            # TODO: Nino Tarantino 2/16/26: we can remove this when we resolve #27.
            ${JEOD_HOME}/..
    )

    target_compile_definitions(jeod
        INTERFACE
            JEOD_UNIT_TEST
    )

    # Determine the version of JEOD that was found.
    execute_process(
        COMMAND git describe --tags --abbrev=0
        WORKING_DIRECTORY ${JEOD_HOME}
        OUTPUT_VARIABLE JEOD_VERSION_FULL
        ERROR_VARIABLE JEOD_VERSION_FULL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (JEOD_VERSION_FULL MATCHES "jeod_v([0-9]+\\.[0-9]+\\.?[0-9]?)")
        # Match JEOD strings that fit the pattern "jeod_vmajor.minor.patch", with the
        # patch being optional, and extract only the version numbers.
        set(JEOD_VERSION ${CMAKE_MATCH_1})
    else()
        # Try to figure out the JEOD release from README_jeod.
        file(READ "${JEOD_HOME}/README_jeod" README_CONTENT)
        string(REGEX MATCH "The JEOD ([0-9]+)\\.([0-9]+)(\\.([0-9]+))? Release" MATCHED_STRING "${README_CONTENT}")

        set(JEOD_VERSION_MAJOR ${CMAKE_MATCH_1})
        set(JEOD_VERSION_MINOR ${CMAKE_MATCH_2})
        set(JEOD_VERSION_PATCH ${CMAKE_MATCH_4})
        if (NOT JEOD_VERSION_PATCH MATCHES "^[0-9]+$")
            set(JEOD_VERSION "${JEOD_VERSION_MAJOR}.${JEOD_VERSION_MINOR}")
        else()
            set(JEOD_VERSION "${JEOD_VERSION_MAJOR}.${JEOD_VERSION_MINOR}.${JEOD_VERSION_PATCH}")
        endif()
    endif()

    # Find libjeod.a for unit tests.
    find_library(JEOD_LIBRARY
        NAMES jeod
        HINTS ${JEOD_HOME}
        PATH_SUFFIXES lib_jeod_${Trick_HOST_CPU}
    )
    mark_as_advanced(JEOD_LIBRARY)
    if (JEOD_LIBRARY)
        # Since we found libjeod, any executable that depends on JEOD will link
        # against it.
        set(JEOD_Library_FOUND TRUE)
        add_library(jeod_library STATIC IMPORTED)
        set_target_properties(jeod_library PROPERTIES IMPORTED_LOCATION ${JEOD_LIBRARY})
        target_link_libraries(jeod_library INTERFACE Trick::Trick)
        target_link_libraries(jeod INTERFACE jeod_library)
    else()
        set(_FAILURE_REASON "Could not find libjeod.a on your system!")
    endif()
else()
    set(_FAILURE_REASON "Could not find JEOD on your system!")
endif()

# Verify that all necessary components have been found.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(JEOD
    REQUIRED_VARS
        JEOD_HOME
        JEOD_VERSION
    VERSION_VAR JEOD_VERSION
    REASON_FAILURE_MESSAGE ${_FAILURE_REASON}
    HANDLE_COMPONENTS
)
