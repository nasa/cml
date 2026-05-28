/**********************TRICK * HEADER******************************
PURPOSE: (Example of a RCS system)

LIBRARY DEPENDENCY:
   ((../src/Example_RcsAdvanced.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (2017) (Antares) (initial)))
**********************************************************************/

#ifndef EXAMPLE_RCS_ADVANCED_HH
#define EXAMPLE_RCS_ADVANCED_HH

#include "rcs_generic.hh"
#include "rcs_prop_pod.hh"
#include "rcs_group.hh"
#include "rcs_jet.hh"

class RcsAdvancedExample : public RcsGeneric
{
 public:
  RcsPropPod  rcs_jets_pod;
  RcsPropPod  main_engine_pod;
  RcsJetGroup stringA;
  RcsJetGroup stringB;
  RcsJetGroup main_engine_group;
  RcsJet rcs_1A;
  RcsJet rcs_2A;
  RcsJet rcs_3A;
  RcsJet rcs_4A;
  RcsJet rcs_5A;
  RcsJet rcs_6A;
  RcsJet rcs_7A;
  RcsJet rcs_8A;
  RcsJet rcs_1B;
  RcsJet rcs_2B;
  RcsJet rcs_3B;
  RcsJet rcs_4B;
  RcsJet rcs_5B;
  RcsJet rcs_6B;
  RcsJet rcs_7B;
  RcsJet rcs_8B;
  RcsJet me_1A;
  RcsJet me_2A;
  RcsJet me_1B;
  RcsJet me_2B;
 protected:
  void configure_model();
  void configure_groups();
  void configure_pods();
  void configure_jets();
 public:
  RcsAdvancedExample();
 private:
   // Not implemented:
   RcsAdvancedExample (const RcsAdvancedExample& rhs);
   RcsAdvancedExample & operator = (const RcsAdvancedExample& rhs);;
};
#endif
