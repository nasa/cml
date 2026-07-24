set(CML_DOCS_OUTPUT_DIRECTORY ${cml_SOURCE_DIR}/build/docs)

find_package(Doxygen)
if (Doxygen_FOUND)
    # Tell Doxygen to only check the sources and headers that we're building. This
    # way we don't bother checking Trick sim outputs or anything like that.
    string(REPLACE ";" " \\\n" CML_DOXYGEN_INPUT_FILES "${CML_HEADER_FILES};${CML_SOURCE_FILES}")

    # Configure the template file using information from CMake and deploy it to
    # the build directory.
    set(CONFIGURED_DOXYFILE ${CML_DOCS_OUTPUT_DIRECTORY}/Doxyfile)
    configure_file(Doxyfile.in ${CONFIGURED_DOXYFILE} @ONLY)

    # Add doxygen target. By doing it this way rather than with doxygen_add_docs()
    # it's easier to place the generated documentation in a shared area.
    add_custom_command(
        OUTPUT ${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml/index.xml
        DEPENDS
            Doxyfile.in
            ${CML_HEADER_FILES}
            ${CML_SOURCE_FILES}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CONFIGURED_DOXYFILE}
        WORKING_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

    add_custom_target(doxygen
        DEPENDS ${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml/index.xml
    )
else()
    message(WARNING "Could not detect Doxygen on your system. Cannot build documentation!")
endif()

#find_package(Sphinx)
#
#if (Sphinx_FOUND)
#    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
#    set(SPHINX_BUILD ${CML_DOCS_OUTPUT_DIRECTORY}/sphinx)
#    set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)
#
#    add_custom_command(
#        OUTPUT ${SPHINX_INDEX_FILE}
#        COMMAND Sphinx::Sphinx -b html -Dbreathe_projects.CML=${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml ${SPHINX_SOURCE} ${SPHINX_BUILD}
#        DEPENDS
#            doxygen
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Generating documentation with Sphinx"
#    )
#    add_custom_target(sphinx
#        DEPENDS ${SPHINX_INDEX_FILE}
#    )
#else()
#    message(WARNING "Could not detect Sphinx on your system. Cannot build documentation!")
#endif()

add_custom_target(documentation
    DEPENDS
        doxygen
        #sphinx
    COMMENT "Building Doxygen and Sphinx documentation"
)
