# We're intentionally using the public-facing CML makefile and not the internal unit
# sim makefile. This is so we can test building all of the CML models in a Trick
# environment.
CML_TRICK_BUILD=1
include ${CML_HOME}/mkspecs/cml.mk

# Still use internal JEOD makefile.
include ${CML_HOME}/mkspecs/internal/jeod.mk

export TRICK_CXXFLAGS += -I${JEOD_HOME}/.. -I${JEOD_HOME}/models -std=c++17
