/*******************************TRICK HEADER******************************
PURPOSE: (
  A crude representation of the sim-side data structures.
  In a real sim, the events-manager would use data from the sim to monitor
  for events triggering. This class provides a collection of meaningless
  values just to test the interface to sim-values.
  )

   PROGRAMMERS:
     (((Gary Turner) (OSR) (Feb 2024) (ANTARES) (model verification)))
     ***********************************************************************/
#ifndef CML_VERIFICATION_COMPOUND_EVENTS_SIM_DATA_HH
#define CML_VERIFICATION_COMPOUND_EVENTS_SIM_DATA_HH
struct SampleSimData
{
  double time;
  double sim_var;
  bool action_assgt;
};
#endif
