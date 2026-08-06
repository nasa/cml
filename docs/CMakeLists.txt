set(CML_DOCS_OUTPUT_DIRECTORY ${cml_SOURCE_DIR}/build/docs)
set(CML_XML_OUTPUT_DIRECTORY ${CML_DOCS_OUTPUT_DIRECTORY}/doxygen/xml)

add_subdirectory(doxygen)
add_subdirectory(sphinx)

if (TARGET Doxygen::doxygen AND TARGET Sphinx::Sphinx)
    add_custom_target(documentation
        DEPENDS
            doxygen
            sphinx
        COMMENT "Building Doxygen and Sphinx documentation"
    )
else()
    if (NOT TARGET Doxygen::doxygen)
        message(WARNING "Doxygen not found. Cannot build documentation.")
    endif()
    if (NOT TARGET Sphinx::Sphinx)
        message(NOTICE "Sphinx not found. Cannot build documentation.")
    endif()
endif()
