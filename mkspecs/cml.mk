# cml.mk
# ------
#
# The following variables can be defined prior to including this makefile in your
# S_overrides.mk to change the default CML build behavior:
#
#     CML_TRICK_BUILD=1    : Build CML models the standard Trick way, by selectively
#                            compiling source files based on the included headers in
#                            your simulation. Otherwise, link against libcml, the pre-
#                            built library containing all CML models, and against the
#                            Trickified library containing prebuilt SWIG and io_src
#                            code. If this value is true (1), none of the following
#                            options have any effect.
#
#     CML_NO_TRICKIFY=1    : Generate Trick memory manager and SWIG interface code at
#                            build-time. Otherwise, use the pre-built Trickified CML
#                            library and generated code.
#
#     CML_OFFLINE_BUILD=1  : Don't attempt to build libcml, simply link against it.
#                            This assumes that you have already built libcml during an
#                            offline configuration step for your repo. Otherwise, build
#                            libcml as part of the sim build process. It is recommended
#                            to set this option to true (1) if you have a workflow with
#                            many parallel builds that depend on CML, to prevent the
#                            parallel builds potentially clobbering each other. Note that
#                            if this value is true (1), the build will fail if libcml
#                            does not already exist.
#
#     CML_BUILD_DEBUG=1    : Link against the unoptimized debug version of libcml.
#                            Otherwise, link against the RelWithDebInfo build, which
#                            has optimizations enabled and enough debug information to
#                            show a proper stack trace if something goes wrong, but
#                            will provide a poor debugging experience in gdb.
#
#     CML_USE_STATIC_LIB=1 : Link against the static version of libcml (libcml.a as
#                            opposed to libcml.so). Useful if you're planning on
#                            deploying your simulation to a different environment than
#                            where it was built. May also be necessary in certain CI
#                            workflows since the run-time repo path of a runner may
#                            change between job stages.
#
# Note that you can also change these options by adding them as arguments following
# the call to trick-CP, for example:
#
#     trick-CP CML_BUILD_DEBUG=1 CML_USE_STATIC_LIB=1

CML_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)

CML_TRICK_BUILD    ?= 0
CML_NO_TRICKIFY    ?= 0
CML_OFFLINE_BUILD  ?= 0
CML_BUILD_DEBUG    ?= 0
CML_USE_STATIC_LIB ?= 0
ifeq (0, ${CML_TRICK_BUILD})
    include ${CML_HOME}/mkspecs/internal/cml_lib.mk
endif

TRICK_CXXFLAGS += -I${CML_HOME}/..
TRICK_CFLAGS += -I${CML_HOME}/..

# Include paths and libraries for libxml2
ifeq ($(shell pkg-config --exists libxml-2.0 && echo $$?),0)
    TRICK_CXXFLAGS += $(shell pkg-config libxml-2.0 --cflags)
    TRICK_LDFLAGS += $(shell pkg-config libxml-2.0 --libs)
endif
