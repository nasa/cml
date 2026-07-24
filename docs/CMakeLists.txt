set(CML_DOCS_OUTPUT_DIRECTORY ${cml_SOURCE_DIR}/build/docs)

find_package(Doxygen)
if (Doxygen_FOUND)
    # Tell Doxygen to only check the sources and headers that we're building. This
    # way we don't bother checking Trick sim outputs or anything like that.
    #set(
    string(REPLACE ";" " \\\n" CML_DOXYGEN_INPUT_FILES "${CML_HEADER_FILES};${CML_SOURCE_FILES}")

    # Configure the template file using information from CMake and deploy it to
    # the build directory.
    set(CONFIGURED_DOXYFILE ${CML_DOCS_OUTPUT_DIRECTORY}/Doxyfile)
    configure_file(Doxyfile.in ${CONFIGURED_DOXYFILE} @ONLY)

    # Add doxygen target. By doing it this way rather than with doxygen_add_docs()
    # it's easier to place the generated documentation in a shared area.
    add_custom_target(doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${CONFIGURED_DOXYFILE}
        WORKING_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(WARNING "Could not detect Doxygen on your system. Cannot build documentation!")
endif()
