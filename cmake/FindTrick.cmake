# FindTrick
# ---------
#
# This module looks for an installation of Trick. The following variables are
# populated for external use:
#
#     Trick_BIN_DIRECTORY : the directory holding Trick scripts like trick-CP
#     Trick_HOME : the Trick root directory
#     Trick_VERSION : the version string for the current Trick release
#     Trick_HOST_CPU : output from trick-gte with the TRICK_HOST_CPU option
#
# Additionally, the following target is created that specifies Trick include
# paths and libraries to link against:
#
#     Trick::Trick
#
# When using find_package(Trick COMPONENTS ...), the following components are
# possible:
#
#     Core    : The core Trick headers and scripts
#     Library : A prebuilt libtrick
#
# We search the following locations for Trick, in order:
#     1. Using the TRICK_HOME environment variable
#     2. Looking one level up from the CML root directory
#     3. Looking in the externals/ directory
#     4. At the system level

set(Trick_Core_FOUND FALSE)
set(Trick_Library_FOUND FALSE)

add_library(trick INTERFACE)
add_library(Trick::Trick ALIAS trick)

find_path(Trick_BIN_DIRECTORY
    NAMES trick-version
    HINTS
        ENV TRICK_HOME
        ${cml_SOURCE_DIR}/../trick
        ${cml_SOURCE_DIR}/externals/trick
    PATH_SUFFIXES bin
)

if (Trick_BIN_DIRECTORY)
    set(Trick_Core_FOUND TRUE)
    file(REAL_PATH ${Trick_BIN_DIRECTORY}/.. Trick_HOME)

    # We'll want the main include directory and trick_source in our include paths.
    find_path(Trick_INCLUDE_DIRECTORY
        NAMES trick/Executive.hh
        HINTS ${Trick_HOME}
        PATH_SUFFIXES include
    )
    find_path(Trick_SOURCE_INCLUDE_DIRECTORY
        NAMES er7_utils
        HINTS ${Trick_HOME}
        PATH_SUFFIXES trick_source
    )
    mark_as_advanced(Trick_INCLUDE_DIRECTORY)
    mark_as_advanced(Trick_SOURCE_INCLUDE_DIRECTORY)

    # Trick include directories are marked as system paths so we don't get any
    # compiler or linter warnings from them.
    target_include_directories(trick SYSTEM
        INTERFACE
            ${Trick_INCLUDE_DIRECTORY}
            ${Trick_INCLUDE_DIRECTORY}/trick/compat
            ${Trick_SOURCE_INCLUDE_DIRECTORY}
    )

    # Determine the version of Trick that was found.
    execute_process(
        COMMAND ${Trick_BIN_DIRECTORY}/trick-version -v
        OUTPUT_VARIABLE Trick_VERSION_FULL
        ERROR_VARIABLE Trick_VERSION_FULL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (Trick_VERSION_FULL MATCHES "^([0-9]+\\.[0-9]+\\.[0-9]+)")
        # Filter out the optional pre-release qualifier that may be at the end of
        # the release tag. For example: "major.minor.patch-beta" becomes "major.minor.patch".
        set(Trick_VERSION ${CMAKE_MATCH_1})
        string(REGEX MATCH "([0-9]+)" Trick_VERSION_MAJOR ${Trick_VERSION})
        mark_as_advanced(Trick_VERSION_MAJOR)
    else()
        set(Trick_VERSION 0.0.0)
        set(Trick_VERSION_MAJOR 0)
    endif()
    target_compile_definitions(trick
        INTERFACE
            TRICK_VER=${Trick_VERSION_MAJOR}
    )

    # Determine the Trick host CPU.
    execute_process(
        COMMAND ${Trick_BIN_DIRECTORY}/trick-gte TRICK_HOST_CPU
        OUTPUT_VARIABLE Trick_HOST_CPU
        ERROR_VARIABLE Trick_HOST_CPU
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Find libtrick.a and its parent directory for unit tests.
    find_library(Trick_LIBRARY
        NAMES trick
        HINTS
            ${Trick_HOME}
        PATH_SUFFIXES
            lib
            lib64
    )
    cmake_path(GET Trick_LIBRARY PARENT_PATH Trick_LIBRARY_DIRECTORY)
    mark_as_advanced(Trick_LIBRARY)
    if (Trick_LIBRARY)
        # Since we found libtrick, any executable that depends on Trick will link
        # against it.
        set(Trick_Library_FOUND TRUE)
        add_library(trick_library STATIC IMPORTED)
        set_target_properties(trick_library PROPERTIES IMPORTED_LOCATION ${Trick_LIBRARY})
        target_link_libraries(trick_library
            INTERFACE
                dl
                pthread
                udunits2
        )
        target_link_libraries(trick INTERFACE trick_library)
    else()
        set(_FAILURE_REASON "Could not find libtrick on your system!")
    endif()
else()
    set(_FAILURE_REASON "Could not find Trick on your system!")
endif()

# The main libtrick also depends on several sub-libraries that we'll need to find
# or else we'll get linker errors.
if (Trick_Library_FOUND)
    # Function to find one of the component libraries that Trick builds in
    # addition to the main libtrick.a, such as libtrick_units.a and liber7_utils.a.
    function(find_trick_sublibrary libname)
        find_library(Trick_lib${libname}
            NAMES ${libname}
            HINTS ${Trick_LIBRARY_DIRECTORY}
        )
        if (Trick_lib${libname})
            add_library(Trick::${libname} STATIC IMPORTED)
            set_target_properties(Trick::${libname}
                PROPERTIES IMPORTED_LOCATION ${Trick_lib${libname}}
            )
            target_link_libraries(trick_library INTERFACE Trick::${libname})
        else()
            # If we can't find one of the Trick sub-libraries, we'll have linker
            # errors, so tell CMake that we couldn't find a complete libtrick
            # after all.
            set(Trick_Library_FOUND FALSE PARENT_SCOPE)
        endif()
    endfunction()

    # Find all the component libraries and declare them as dependencies of the
    # main Trick library.
    find_trick_sublibrary(er7_utils)
    find_trick_sublibrary(trick_comm)
    find_trick_sublibrary(trick_math)
    find_trick_sublibrary(trick_pyip)
    find_trick_sublibrary(trick_var_binary_parser)
    find_trick_sublibrary(trick_connection_handlers)
    find_trick_sublibrary(trick_mm)
    find_trick_sublibrary(trick_units)

    # Trick static libraries depend on each other. Without these steps, the
    # linker may complain about unresolved symbols.
    target_link_libraries(Trick::trick_mm INTERFACE trick_library)
    target_link_libraries(Trick::trick_pyip INTERFACE trick_library)
endif()

# Verify that all necessary components have been found.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Trick
    REQUIRED_VARS
        Trick_HOME
        Trick_BIN_DIRECTORY
        Trick_INCLUDE_DIRECTORY
        Trick_SOURCE_INCLUDE_DIRECTORY
        Trick_HOST_CPU
        Trick_VERSION
    VERSION_VAR Trick_VERSION
    REASON_FAILURE_MESSAGE ${_FAILURE_REASON}
    HANDLE_COMPONENTS
)
