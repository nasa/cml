set(CML_DOCS_OUTPUT_DIRECTORY ${cml_SOURCE_DIR}/build/docs)
set(CML_XML_OUTPUT_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml)

find_package(Doxygen)
if (Doxygen_FOUND)
    # Tell Doxygen to only check the sources and headers that we're building. This
    # way we don't bother checking Trick sim outputs or anything like that.
    set(CML_DOXYGEN_INPUTS ${CML_HEADER_FILES} ${CML_SOURCE_FILES})

    # Don't process huge data files with Doxygen. They don't have Doxygen metadata in them.
    list(FILTER CML_DOXYGEN_INPUTS EXCLUDE REGEX "/(GRGM900C_lunar_gravity|egm_2008_data)\\.cc$")
    string(REPLACE ";" " \\\n" CML_DOXYGEN_INPUT_FILES "${CML_DOXYGEN_INPUTS}")

    # Configure the template file using information from CMake and deploy it to
    # the build directory.
    set(CONFIGURED_DOXYFILE ${CML_DOCS_OUTPUT_DIRECTORY}/Doxyfile)
    configure_file(Doxyfile.in ${CONFIGURED_DOXYFILE} @ONLY)

    # Script that restores literal "--" in the generated xml. By default, Doxygen
    # converts these to em-dashes.
    set(CML_FIX_DASHES_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/fix_em_dashes.sh)

    # Lifts Trick units out of member descriptions into a dedicated <trickunits>
    # element.
    set(CML_EXTRACT_UNITS_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/extract_trick_units.py)

    # Add doxygen target. By doing it this way rather than with doxygen_add_docs()
    # it's easier to place the generated documentation in a shared area.
    set(DOXYGEN_XML_OUTPUT_INDEX ${CML_XML_OUTPUT_DIRECTORY}/index.xml)
    add_custom_command(
        OUTPUT ${DOXYGEN_XML_OUTPUT_INDEX}
        DEPENDS
            Doxyfile.in
            ${CML_DOXYGEN_INPUTS}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CONFIGURED_DOXYFILE}
        COMMAND bash ${CML_FIX_DASHES_SCRIPT} ${CML_XML_OUTPUT_DIRECTORY}
        COMMAND python3 ${CML_EXTRACT_UNITS_SCRIPT} ${CML_XML_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
    add_custom_target(doxygen
        DEPENDS ${DOXYGEN_XML_OUTPUT_INDEX}
    )
else()
    message(WARNING "Could not detect Doxygen on your system. Cannot build documentation!")
endif()

find_package(Sphinx)
if (Sphinx_FOUND)
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/sphinx)
    set(SPHINX_BUILD ${CML_DOCS_OUTPUT_DIRECTORY}/sphinx)
    set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

    add_custom_command(
        OUTPUT ${SPHINX_INDEX_FILE}
        COMMAND Sphinx::Sphinx -b html -Dbreathe_projects.CML=${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml ${SPHINX_SOURCE} ${SPHINX_BUILD}
        DEPENDS
            doxygen
            Doxyfile.in
            ${DOXYGEN_XML_OUTPUT_INDEX}
            ${CML_DOXYGEN_INPUTS}
            CML_Coding_Standards.rst
            sphinx/conf.py
            sphinx/index.rst
            sphinx/src/contributing/contributing.rst
            sphinx/src/contributing/coding-standards.rst
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Sphinx"
    )
    add_custom_target(sphinx
        DEPENDS ${SPHINX_INDEX_FILE}
    )
else()
    message(WARNING "Could not detect Sphinx on your system. Cannot build documentation!")
endif()

add_custom_target(documentation
    DEPENDS
        doxygen
        sphinx
    COMMENT "Building Doxygen and Sphinx documentation"
)
