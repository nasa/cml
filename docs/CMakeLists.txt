set(CML_DOCS_OUTPUT_DIRECTORY ${cml_SOURCE_DIR}/build/docs)
set(CML_XML_OUTPUT_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml)

add_subdirectory(doxygen)

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
            ${DOXYGEN_XML_OUTPUT_INDEX}
            ${CML_DOXYGEN_INPUTS}
            CML_Coding_Standards.rst
            sphinx/conf.py
            sphinx/index.rst
            sphinx/src/contributing/building-the-docs.rst
            sphinx/src/contributing/contributing.rst
            sphinx/src/contributing/coding-standards.rst
            sphinx/src/contributing/getting-started.rst
            sphinx/src/models/utilities/index.rst
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