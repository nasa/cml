# Unfortunately we don't have access to the JOB_SERVER_AWARE argument to
# add_custom_command until cmake 3.28. This is the solution for now so that the
# build still completes in a reasonable amount of time.
if (${CMAKE_GENERATOR} MATCHES "Makefile")
    set(_TRICKIFY_MAKE_COMMAND "\$(MAKE)")
else()
    set(_TRICKIFY_MAKE_COMMAND "make")
endif()

set(CML_TRICKIFIED_OBJECT ${cml_SOURCE_DIR}/trickified/cml_trickified.o)
set(CML_TRICKIFIED_PYTHON ${cml_SOURCE_DIR}/trickified/python.zip)
set(CML_TRICKIFIED_TRICK_DIR ${cml_SOURCE_DIR}/trickified/.trick)
set(CML_TRICKIFIED_BUILD_DIR ${cml_SOURCE_DIR}/trickified/build)
set(CML_TRICKIFIED_BYPRODUCTS ${cml_SOURCE_DIR}/trickified/full_file)

# In our top-level CMake script we consolidated a list of all headers in CML into
# the CML_HEADER_FILES variable. We use that as our list of dependencies for re-
# Trickifying the project.
add_custom_command(
    OUTPUT ${CML_TRICKIFIED_OBJECT}
    COMMAND ${CMAKE_COMMAND} -E env TRICK_HOME=${Trick_HOME} JEOD_HOME=${JEOD_HOME} ${_TRICKIFY_MAKE_COMMAND}
    DEPENDS
        makefile
        S_source.hh
        ${CML_HEADER_FILES}
    WORKING_DIRECTORY ${cml_SOURCE_DIR}/trickified
    BYPRODUCTS
        ${CML_TRICKIFIED_PYTHON}
        ${CML_TRICKIFIED_TRICK_DIR}
        ${CML_TRICKIFIED_BUILD_DIR}
        ${CML_TRICKIFIED_BYPRODUCTS}
    COMMENT "Building CML Trickified library"
)
add_custom_target(trickify
    DEPENDS ${CML_TRICKIFIED_OBJECT}
)

install(
    FILES ${CML_TRICKIFIED_OBJECT} ${CML_TRICKIFIED_PYTHON}
    DESTINATION ${CML_INSTALL_DIR}/trickified
    OPTIONAL
)
install(
    DIRECTORY ${CML_TRICKIFIED_TRICK_DIR} ${CML_TRICKIFIED_BUILD_DIR}
    DESTINATION ${CML_INSTALL_DIR}/trickified
    MESSAGE_NEVER
    OPTIONAL
)
