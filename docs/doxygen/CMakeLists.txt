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
    # converts these to en-dashes.
    set(CML_FIX_DASHES_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/fix_en_dashes.py)

    # Script that pulls Trick units out of member descriptions into a dedicated
    # <trickunits> element. We use this to move the unit descriptions to their own
    # dedicated section of member variable descriptions in the generated HTML.
    set(CML_EXTRACT_UNITS_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/extract_trick_units.py)

    # Add doxygen target. By doing it this way rather than with doxygen_add_docs()
    # it's easier to place the generated documentation in a shared area.
    set(DOXYGEN_XML_OUTPUT_INDEX ${CML_XML_OUTPUT_DIRECTORY}/index.xml)
    add_custom_command(
        OUTPUT ${DOXYGEN_XML_OUTPUT_INDEX}
        DEPENDS
            Doxyfile.in
            extract_trick_units.py
            fix_en_dashes.py
            ${CML_DOXYGEN_INPUTS}
        COMMAND ${DOXYGEN_EXECUTABLE} ${CONFIGURED_DOXYFILE}
        COMMAND python3 ${CML_FIX_DASHES_SCRIPT} ${CML_XML_OUTPUT_DIRECTORY}
        COMMAND python3 ${CML_EXTRACT_UNITS_SCRIPT} ${CML_XML_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
    add_custom_target(doxygen
        DEPENDS ${DOXYGEN_XML_OUTPUT_INDEX}
    )
else()
    message(WARNING "Could not detect Doxygen on your system. Will not be able to build documentation!")
endif()
