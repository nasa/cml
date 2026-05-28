/*******************************************************************************
PURPOSE:
  (Simulates the effects of RCS thruster build-up and trail-off.)

PROGRAMMERS:
  (((Michael McCarthy) (OSR) (Jul 2019) (ANTARES) (CM RCS Refactor, removed C
                             interfacing, split scale factors model into
                             sub-models))
   ((Daniel Ghan)      (OSR) (Mar 2020) (ANTARES) (Added Scale Factor
                             Interface, split CML model from CEV model)))
******************************************************************************/

#include "../include/rcs_build_trail.hh"

#include <algorithm> // min
#include <cstdlib> // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"

RcsBuildUpTrailOff::RcsBuildUpTrailOff( RcsScaleFactorInterface& interface_,
                                        RcsBuildUpTrailOffJetData * const jet_,
                                        const double& time)
  :
  interface(interface_),
  jet(jet_),
  current_time(time),
  active(false)
{
  // NULL check
  if (jet == NULL)
  {
    CMLMessage::fail(__FILE__, __LINE__, "Invalid pointer\n",
      "Pointer to jet data is NULL! Cannot initialize RcsBuildUpTrailOff.\n");
  }
}

void RcsBuildUpTrailOff::build_up_trail_off_effects()
{
  if (!active) return;

  for (unsigned int id = 0; id < interface.num_jets; id++)
  {
    if (interface.get_jet(id)->rcs_valve_open)
    {
      jet[id].tf_build_up = std::min(
          ((current_time - interface.get_jet(id)->rcs_thrusting_start_time) /
           (jet[id].rise_time - interface.valve_rise_time)), 1.0);
    }
    else
    {
      jet[id].tf_trail_off = std::min(
          ((current_time - interface.get_jet(id)->rcs_thrusting_stop_time) /
           (jet[id].decay_time - interface.valve_decay_time)), 1.0);
    }
  }
}