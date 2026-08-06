find_package(Sphinx)

if (TARGET Doxygen::doxygen AND Sphinx_FOUND)
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
    set(SPHINX_BUILD ${CML_DOCS_OUTPUT_DIRECTORY}/sphinx)
    set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

    add_custom_command(
        OUTPUT ${SPHINX_INDEX_FILE}
        COMMAND Sphinx::Sphinx -b html -Dbreathe_projects.CML=${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml ${SPHINX_SOURCE} ${SPHINX_BUILD}
        DEPENDS
            doxygen
            ${DOXYGEN_XML_OUTPUT_INDEX}
            ${CML_DOXYGEN_INPUTS}
            conf.py
            index.rst
            Makefile
            _static/cml_theme.css
            _static/trickunits.css
            _static/trickunits.js
            src/contributing/building-the-docs.rst
            src/contributing/coding-standards.rst
            src/contributing/contributing.rst
            src/contributing/getting-started.rst
            src/models/utilities/index.rst
            src/models/utilities/subscriptions.rst
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Sphinx"
    )
    add_custom_target(sphinx
        DEPENDS ${SPHINX_INDEX_FILE}
    )
endif()
