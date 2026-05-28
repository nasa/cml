/**********************TRICK * HEADER******************************
PURPOSE: (Example of a RCS system.  For testing purpsoes only.)

LIBRARY DEPENDENCY:
   ((../src/rcs_test_multigroup.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (date) (project) (comment)))
**********************************************************************/

#ifndef RCS_TEST_MULTIGROUP_HH
#define RCS_TEST_MULTIGROUP_HH

#include "trick/units_conv.h" /* for unit conversion */
#include "../../../include/rcs_generic_classes.hh"

class RcsTestMultigroup : public RcsGeneric
{
 public:
  RcsPropPod  rcs_pod_1;
  RcsPropPod  rcs_pod_2;
  RcsJetGroup group_1;
  RcsJetGroup group_2;
  RcsJet      jet_01;
  RcsJet      jet_02;
  RcsJet      jet_03;
  RcsJet      jet_04;

  void default_data();
 protected:
  void configure_model();
  void configure_groups();
  void configure_pods();
  void configure_jets();
 public:
  RcsTestMultigroup();
 private:
   // Not implemented:
   RcsTestMultigroup (const RcsTestMultigroup& rhs);
   RcsTestMultigroup & operator = (const RcsTestMultigroup& rhs);;
};
#endif    
