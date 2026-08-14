# cml_unit_sim.mk
# ---------------
#
# This file is intended to be included by all unit sims in the CML repo. It sets
# default build arguments that we want for testing content in CML. It is not
# intended for external use.

CML_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)

CML_BUILD_DEBUG := 1
include ${CML_HOME}/mkspecs/cml.mk
include ${CML_HOME}/mkspecs/internal/jeod.mk

# temporary fix for Mac as -Wno-nonnull-compare is not recognized by clang
TRICK_CFLAGS += -g -I. -Wall -Wextra -Werror -Wno-implicit-fallthrough -Wno-format-truncation -Wno-int-in-bool-context -Wno-address
TRICK_CXXFLAGS += -g -I. -Wall -Wextra -Werror -Wno-implicit-fallthrough -Wno-format-truncation -Wno-int-in-bool-context -Wno-address
TRICK_SFLAGS += -I${JEOD_HOME}/lib/jeod/JEOD_S_modules -I.

# Try adding -Wno-nonnull-compare only if the compiler supports it
CHECK_CXXFLAG = $(shell \
    $(CXX) -Werror=unknown-warning-option -Wno-nonnull-compare -E - < /dev/null \
	>/dev/null 2>&1 && echo yes || echo no)

ifeq ($(CHECK_CXXFLAG),yes)
TRICK_CFLAGS += -Wno-nonnull-compare
TRICK_CXXFLAGS += -Wno-nonnull-compare
endif

# Remove after JEOD 5:
TRICK_CXXFLAGS += -I${JEOD_HOME}/models -I${JEOD_HOME}/..

# Strip unneeded -lint from TRICK_EXEC_LINK_LIBS. This originates from python3-config
# and appears to be part of the FSL spack-installed version of python3.X only
# If we can understand/resolve the core issue, this can be removed.
# See: https://gitlab-fsl.jsc.nasa.gov/FSL/Documentation/-/issues/1111  -Jordan 4/2024
TRICK_EXEC_LINK_LIBS := $(filter-out -lintl,$(TRICK_EXEC_LINK_LIBS))
TRICK_CFLAGS += -g -I. -Wall -Wextra -Werror
TRICK_CXXFLAGS += -g -I. -std=c++17 -Wall -Wextra -Werror
TRICK_SFLAGS += -I${JEOD_HOME}/lib/jeod/JEOD_S_modules -I.

# Sanitizer options for unit sims.
TRICK_CFLAGS += -fsanitize=address,undefined
TRICK_CXXFLAGS += -fsanitize=address,undefined
TRICK_LDFLAGS += -fsanitize=address,undefined

TRICK_CXXFLAGS += -I${JEOD_HOME}/..

# ============================================================
# macOS (Darwin) specific fixes
# ============================================================
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)

# ------------------------------------------------------------
# 1. Strip Trick's auto-added -std=c++11 and -std=c++14
# 2. Add -Wno-overloaded-virtual to Apple builds
# ------------------------------------------------------------
TRICK_IO_CXXFLAGS := $(filter-out -std=c++11 -std=c++14,$(TRICK_IO_CXXFLAGS))
TRICK_IO_CXXFLAGS += -std=c++17

TRICK_CXXFLAGS := $(filter-out -std=c++11 -std=c++14,$(TRICK_CXXFLAGS))
TRICK_CXXFLAGS += -std=c++17

TRICK_CXXFLAGS += -Wno-overloaded-virtual
TRICK_CFLAGS   += -Wno-overloaded-virtual
TRICK_IO_CXXFLAGS += -Wno-overloaded-virtual

endif
