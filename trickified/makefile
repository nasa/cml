TRICKIFY := $(TRICK_HOME)/share/trick/makefiles/trickify.mk

INSTALL_DIR ?= $(abspath $(lastword $(MAKEFILE_LIST)))

export TRICKIFY_OBJECT_NAME := cml_trickified.o
export TRICKIFY_CXX_FLAGS := -I${CML_HOME}/.. -I${JEOD_HOME}/.. -I${JEOD_HOME}/models -isystem${TRICK_HOME}/include -isystem${TRICK_HOME}/trick_source -O2 -g -w
export TRICKIFY_EXT_LIB_DIRS += :${JEOD_HOME}/models
export TRICKIFY_PYTHON_DIR := python.zip
export ICG_EXCLUDE += :${JEOD_HOME}/models

ifeq ($(shell pkg-config --exists libxml-2.0 && echo $$?),0)
    export TRICKIFY_CXX_FLAGS += $(shell pkg-config libxml-2.0 --cflags)
endif

# Due to limitations with Trickification and SWIG, the JEOD Trickified library
# and build artifacts MUST exist for CML to Trickify properly.
ifeq ($(wildcard ${JEOD_HOME}/trickified/trickified_jeod*.o),)
    $(error "Could not find JEOD Trickified library! CML Trickification process will not work correctly.")
endif

export TRICK_SWIG_FLAGS += -I${JEOD_HOME}/trickified

all: trickify_cml

trickify_cml:
	@$(MAKE) -s -f $(TRICKIFY)
	@echo -e "\033[32mTrickified CML library built successfully\033[0m"

clean:
	@rm -rf build ${TRICKIFY_PYTHON_DIR} full_file .trick $(TRICKIFY_OBJECT_NAME)
