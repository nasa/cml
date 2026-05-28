# NOTE: do not include this file directly in your S_overrides.mk! Please include
# cml.mk instead.
#
# This makefile uses the following user-defind options:
#
#     CML_OFFLINE_BUILD
#     CML_NO_TRICKIFY
#     CML_BUILD_DEBUG
#     CML_USE_STATIC_LIB
#
# See cml.mk for explanations of these options.

TRICK_HOST_CPU ?= $(shell ${TRICK_HOME}/bin/trick-gte TRICK_HOST_CPU)
CML_HOME ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
TRICK_GTE_EXT += :${CML_HOME}/models

ifeq (1, ${CML_BUILD_DEBUG})
    CML_BUILD_DIR = ${CML_HOME}/build/debug
    CML_LIBRARY_DIR = ${CML_HOME}/lib/${TRICK_HOST_CPU}/Debug
    CML_CMAKE_PRESET = user-debug
else
    CML_BUILD_DIR = ${CML_HOME}/build/release
    CML_LIBRARY_DIR = ${CML_HOME}/lib/${TRICK_HOST_CPU}/RelWithDebInfo
    CML_CMAKE_PRESET = user-release
endif
CML_TRICKIFIED_DIR = ${CML_HOME}/lib/${TRICK_HOST_CPU}/trickified
CML_TRICKIFIED_LIB = ${CML_TRICKIFIED_DIR}/cml_trickified.o

# Add rule to build libcml if the "offline build" option has not been specified.
ifeq (0, ${CML_OFFLINE_BUILD})
libcml_build:
	@if [ ! -f ${CML_BUILD_DIR}/Makefile ]; then \
		cmake -S ${CML_HOME} --preset ${CML_CMAKE_PRESET}; \
	fi
	@if [ "${CML_NO_TRICKIFY}" == "0" ]; then \
		$(MAKE) -C ${CML_BUILD_DIR} trickify; \
	fi
	$(MAKE) -C ${CML_BUILD_DIR} install

$(S_MAIN): libcml_build
endif

# Add rules for linking against the Trickified CML library and building it, if
# applicable.
ifeq (0, ${CML_NO_TRICKIFY})
    # Find each include directory and tell Trick that it is an external directory,
    # so Trick will not attempt to generate interface code for our types and functions.
    CML_INCLUDE_DIRS := \
        $(shell \
            find ${CML_HOME}/models -name verif -prune -o -name include -print \
            | sort | tr '\n' ':' | sed 's/\(.*\):/\1 /')

    TRICK_EXT_LIB_DIRS += :${CML_INCLUDE_DIRS}
    TRICK_SWIG_FLAGS += -I${CML_TRICKIFIED_DIR}
    TRICK_PYTHON_PATH += :${CML_TRICKIFIED_DIR}/python.zip
    TRICK_LDFLAGS += ${CML_TRICKIFIED_LIB}

ifeq (0, ${CML_OFFLINE_BUILD})
libcml_trickify:
	@echo -e "\033[32mBeginning Trickification of CML\033[0m"
	@$(MAKE) -C ${CML_HOME}/trickified

# If we're Trickifying CML as part of the main sim build, we need to ensure that
# Trickification happens before SWIG is run on any of the main sim's .i files AND
# before the CML library build, since the final library build step installs both
# libcml and the Trickified products.
$(SWIG_SRC): libcml_trickify libcml_build
libcml_build: libcml_trickify
else

# Use offline build but check that it exists
CML_TRICKIFIED_EXISTS:=$(shell if [ -e ${CML_TRICKIFIED_LIB} ]; then echo 1; else echo 0; fi)
ifeq (${CML_TRICKIFIED_EXISTS},0)
$(error CML_OFFLINE_BUILD=${CML_OFFLINE_BUILD} but CML trickified library was not found at ${CML_TRICKIFIED_LIB})
endif
endif

endif

# Find each src/ directory and tell Trick that it is an external directory, so
# Trick will not attempt to compile our source files.
CML_SOURCE_DIRS := \
    $(shell \
        find ${CML_HOME}/models -name verif -prune -o -name src -print \
	| sort | tr '\n' ':' | sed 's/\(.*\):/\1 /')
TRICK_EXT_LIB_DIRS += :${CML_SOURCE_DIRS}

TRICK_LDFLAGS += -L${CML_LIBRARY_DIR}
ifeq (1, $(CML_USE_STATIC_LIB))
    TRICK_LDFLAGS += -l:libcml.a
else
    TRICK_LDFLAGS += -Wl,-rpath=${CML_LIBRARY_DIR} -lcml
endif
