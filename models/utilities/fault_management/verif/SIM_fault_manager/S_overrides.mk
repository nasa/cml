TRICK_CXXFLAGS += $(shell pkg-config libxml-2.0 --cflags)
TRICK_USER_LINK_LIBS += $(shell pkg-config libxml-2.0 --libs)
include ${CML_HOME}/mkspecs/internal/cml_unit_sim.mk
