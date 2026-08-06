find_program(Sphinx_EXECUTABLE NAMES sphinx-build)

if (Sphinx_EXECUTABLE)
    execute_process(
        COMMAND ${Sphinx_EXECUTABLE} --version
        OUTPUT_VARIABLE Sphinx_VERSION
        ERROR_VARIABLE Sphinx_VERSION
    )
    if (Sphinx_VERSION MATCHES "sphinx-build ([0-9]+\\.[0-9]+\\.[0-9]+)")
        set(Sphinx_VERSION_STRING "${CMAKE_MATCH_1}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx
    REQUIRED_VARS Sphinx_EXECUTABLE
    VERSION_VAR Sphinx_VERSION_STRING
)

if (Sphinx_FOUND AND NOT TARGET Sphinx::Sphinx)
    mark_as_advanced(Sphinx_EXECUTABLE)
    add_executable(Sphinx::Sphinx IMPORTED GLOBAL)
    set_property(TARGET Sphinx::Sphinx PROPERTY IMPORTED_LOCATION ${Sphinx_EXECUTABLE})
endif()
