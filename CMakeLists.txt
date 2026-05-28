# ---------------------------------------------------------------------------- #
# CMake build file for the JSC Engineering EG Division Common Model Library    #
#                                                                              #
# This build file requires at minimum CMake 3.24 because of the following      #
# feature(s):                                                                  #
#     The COMPILE_WARNING_AS_ERROR target property.                            #
# ---------------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.24..3.26)

project(cml
    VERSION 0.2.0
    DESCRIPTION "The JSC Engineering EG Division Common Model Library"
    HOMEPAGE_URL "https://github.com/nasa/cml"
    LANGUAGES C CXX
)
message(STATUS "Configuring Common Model Library (CML) CMake build")

list(APPEND CMAKE_MODULE_PATH "${cml_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------- #
# Specify CML build options                                                    #
# ---------------------------------------------------------------------------- #
option(CML_BUILD_UNIT_TESTS
"Build unit tests. Note that this requires Trick and JEOD libraries to be built \
beforehand, and the CMake configuration step will fail if libtrick or libjeod \
cannot be found." OFF)

option(CML_CODE_COVERAGE "Instrument the build to enable coverage profiling." OFF)

option(CML_WARNINGS_AS_ERRORS "Treat all compiler warnings as errors." OFF)

# ---------------------------------------------------------------------------- #
# Locate common CML dependencies:                                              #
#                                                                              #
#     Trick: NASA's simulation framework, which many CML models depend on      #
#     JEOD: JSC Engineering Orbital Dynamics, which many CML models either     #
#           depend on or extend                                                #
#                                                                              #
# Also locate dependencies for CML sub-libraries:                              #
#                                                                              #
#     LibXml2: Required by the XML Helper model                                #
#                                                                              #
# The final dependency is googletest for our unit tests, which we declare in   #
# the externals directory. See below for an explanation.                       #
# ---------------------------------------------------------------------------- #
find_package(Trick 25.0.2 REQUIRED COMPONENTS Core OPTIONAL_COMPONENTS Library)
find_package(JEOD 5.0 REQUIRED COMPONENTS Core OPTIONAL_COMPONENTS Library)
find_package(LibXml2 REQUIRED)

if (NOT JEOD_Library_FOUND AND CML_BUILD_UNIT_TESTS)
    message(FATAL_ERROR "Could not find a pre-built libjeod, which is mandatory for unit tests")
endif()
if (NOT Trick_Library_FOUND AND CML_BUILD_UNIT_TESTS)
    message(FATAL_ERROR "Could not find one or more of the prebuilt Trick libraries, which are mandatory for unit tests")
endif()

# Specify the installation directory, which depends on the TRICK_HOST_CPU.
set(CML_INSTALL_DIR ${cml_SOURCE_DIR}/lib/${Trick_HOST_CPU})

# Below we include the externals directory, which has instructions for grabbing
# googletest if testing is enabled. We have to split this into its own sub-
# directory to provide the EXCLUDE_FROM_ALL flag, which prevents CMake from
# trying to install googletest at the system level when we run the `install`
# target.
add_subdirectory(externals EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------- #
# Enable unit tests                                                            #
# ---------------------------------------------------------------------------- #
if (CML_BUILD_UNIT_TESTS)
    enable_testing()
endif()

# ---------------------------------------------------------------------------- #
# Build libcml as both a static and shared library.                            #
#                                                                              #
# The libraries are built by combining the various sub-libraries of CML. Each  #
# model directory has an associated sub-library.                               #
#                                                                              #
# Note: we declare the targets for the main CML library first, without any     #
# sources. CML is fairly inter-dependent, so our unit tests link to the main   #
# libcml, rather than the corresponding sub-library. So these targets must     #
# exist ahead of time. We add the sources to the libcml targets further down.  #
# ---------------------------------------------------------------------------- #
add_library(cml_static STATIC)
add_library(cml::cml_static ALIAS cml_static)
set_target_properties(cml_static PROPERTIES OUTPUT_NAME cml)

add_library(cml_shared SHARED)
add_library(cml::cml_shared ALIAS cml_shared)
set_target_properties(cml_shared PROPERTIES OUTPUT_NAME cml)

# Iterate over each CML component (i.e., model directory). Include the associated
# CMake file and add the sub-library as a target object for the main libcml.
set(CML_HEADER_FILES)
set(CML_COMPONENTS
    dynamics
    environment
    fhw
    interactions
    tools
    utilities
    vehicle_management
)
foreach (component ${CML_COMPONENTS})
    add_subdirectory(models/${component})
    target_sources(cml_static PRIVATE $<TARGET_OBJECTS:cml::${component}>)
    target_sources(cml_shared PRIVATE $<TARGET_OBJECTS:cml::${component}>)
    get_target_property(_COMPONENT_HEADERS cml::${component} HEADER_SET)
    list(APPEND CML_HEADER_FILES ${_COMPONENT_HEADERS})
endforeach()

# Set library dependencies for libcml. This is mostly for unit test executables
# that rely on libcml.
target_link_libraries(cml_static
    PUBLIC
        LibXml2::LibXml2
    PRIVATE
        $<$<BOOL:${CML_CODE_COVERAGE}>:--coverage>
)
target_link_libraries(cml_shared
    PUBLIC
        LibXml2::LibXml2
    PRIVATE
        $<$<BOOL:${CML_CODE_COVERAGE}>:--coverage>
)

# Set include directories for libcml. This is mostly for unit test executables
# that rely on libcml.
target_include_directories(cml_shared
    INTERFACE
        ${cml_SOURCE_DIR}/..
)
target_include_directories(cml_static
    INTERFACE
        ${cml_SOURCE_DIR}/..
)

# ---------------------------------------------------------------------------- #
# Create Trickification target                                                 #
# ---------------------------------------------------------------------------- #
add_subdirectory(trickified)

# ---------------------------------------------------------------------------- #
# Set up installation targets                                                  #
# ---------------------------------------------------------------------------- #
install(
    TARGETS
        cml_static
        cml_shared
    DESTINATION
        ${CML_INSTALL_DIR}/${CMAKE_BUILD_TYPE}
)
