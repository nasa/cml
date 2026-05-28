# cml_unit_sim.mk
# ---------------
#
# This file is intended to be included by all unit sims in the CML repo. It sets
# default build arguments that we want for testing to content in CML. It is not
# intended for external use.

CML_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)

CML_BUILD_DEBUG := 1
include ${CML_HOME}/mkspecs/cml.mk
include ${CML_HOME}/mkspecs/internal/jeod.mk

TRICK_CFLAGS += -g -I. -Wall -Wextra -Werror -Wno-implicit-fallthrough -Wno-format-truncation -Wno-int-in-bool-context -Wno-address -Wno-nonnull-compare
TRICK_CXXFLAGS += -g -I. -std=c++17 -Wall -Wextra -Werror -Wno-implicit-fallthrough -Wno-format-truncation -Wno-int-in-bool-context -Wno-address -Wno-nonnull-compare
TRICK_SFLAGS += -I${JEOD_HOME}/lib/jeod/JEOD_S_modules -I.

TRICK_CXXFLAGS += -I${JEOD_HOME}/..
