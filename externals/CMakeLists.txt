# ---------------------------------------------------------------------------- #
# CMake build file for external dependencies in CML                            #
#                                                                              #
# Right now the only dependency we need to worry about here is googletest. We  #
# split this into its own module so we can use the EXCLUDE_FROM_ALL argument   #
# with add_subdirectory. That prevents CMake from trying to install googletest #
# in /usr/include when running the `install` target.                           #
#                                                                              #
# There's a more elegant way to do this with the EXCLUDE_FROM_ALL argument for #
# FetchContent_Declare, but that's not available until cmake 3.28.             #
# ---------------------------------------------------------------------------- #

include(FetchContent)

if (CML_BUILD_UNIT_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
endif()
