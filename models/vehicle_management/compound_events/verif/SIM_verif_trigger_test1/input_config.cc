#include "S_source.hh"
#include "trick/CheckPointRestart_c_intf.hh"
#include "trick/realtimesync_proto.h"
#include "trick/external_application_c_intf.h"

/*****************************************************************************
run_*
Purpose: Define the settings for each run.
*****************************************************************************/
extern "C"
void run_1b() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::EQ;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1c() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::NE;
  verif.manager.trigger_i.set_reference(0);
}

extern "C"
void run_1d() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::GT;
  verif.manager.trigger_i.set_reference(2);
}

extern "C"
void run_1e() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::LT;
  verif.manager.trigger_i.set_reference(0);
}

extern "C"
void run_1f() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::GE;
  verif.manager.trigger_i.set_reference(3);
}

extern "C"
void run_1g() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::LE;
  verif.manager.trigger_i.set_reference(0);
}

extern "C"
void run_1h() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_EQ;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1i() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_GT;
  verif.manager.trigger_i.set_reference(4);
}

extern "C"
void run_1j() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_GE;
  verif.manager.trigger_i.set_reference(4);
}

extern "C"
void run_1k() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_LT;
  verif.manager.trigger_i.set_reference(2);
}

extern "C"
void run_1l() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_LE;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1m() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::OC;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1n() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::Change;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1o() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::Crossing;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1p() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MAX_FIRST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1q() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MIN_FIRST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1s() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MAX_FIRST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1t() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MIN_FIRST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1u() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MAX_CONDITIONAL_FIRST;
  verif.manager.trigger_i.set_reference(-1);
}

extern "C"
void run_1v() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MIN_CONDITIONAL_FIRST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1w() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MAX_CONDITIONAL_FIRST;
  verif.manager.trigger_i.set_reference(-1);
}

extern "C"
void run_1x() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MIN_CONDITIONAL_FIRST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1y() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MAX_LAST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1z() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MIN_LAST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1aa() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MAX_LAST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1ab() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MIN_LAST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1ac() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MAX_CONDITIONAL_LAST;
  verif.manager.trigger_i.set_reference(-1);
}

extern "C"
void run_1ad() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::MIN_CONDITIONAL_LAST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1ae() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MAX_CONDITIONAL_LAST;
  verif.manager.trigger_i.set_reference(-1);
}

extern "C"
void run_1af() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_MIN_CONDITIONAL_LAST;
  verif.manager.trigger_i.set_reference(1);
}

extern "C"
void run_1ag() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_EQ;
  verif.manager.trigger_i.set_reference(1);
  verif.manager.trigger_i.direction_limit = EventTriggerBase::Increasing;
}

extern "C"
void run_1ah() {
  verif.manager.trigger_i.comparison_logic = EventTriggerBase::ABS_EQ;
  verif.manager.trigger_i.set_reference(1);
  verif.manager.trigger_i.direction_limit = EventTriggerBase::Decreasing;
}

