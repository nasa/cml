/*******************************************************************************
PURPOSE:
  (Simulates the effects of RCS thruster build-up and trail-off.)

LIBRARY DEPENDENCY:
  ((../src/rcs_build_trail.cc))

PROGRAMMERS:
  (((Michael McCarthy) (OSR) (Jul 2019) (ANTARES) (CM RCS Refactor, removed C
                             interfacing, split scale factors model into
                             sub-models))
   ((Daniel Ghan)      (OSR) (Mar 2020) (ANTARES) (Added Scale Factor
                             Interface, split CML model from CEV model)))
******************************************************************************/

#ifndef RCS_BUILD_TRAIL_HH
#define RCS_BUILD_TRAIL_HH

/*****************************************************************************
RcsBuildUpTrailOffJetData
Purpose:(Jet-specific data for RcsBuildUpTrailOff)
*****************************************************************************/
class RcsBuildUpTrailOffJetData
{
 public:
  double rise_time; /* (s) Rise time */
  double decay_time; /* (s) Decay time */
  double decay_time_abort; /* (s) Decay time when post LAS abort */
  double tf_build_up; /* (--) Thrust factor from the build-up model */
  double tf_trail_off; /* (--) Thrust factor from the trail-off model */

  RcsBuildUpTrailOffJetData()
  :
    rise_time(0.0),
    decay_time(0.0),
    decay_time_abort(0.0),
    tf_build_up(1.0),
    tf_trail_off(1.0)
  {}
};

#include "rcs_scale_factor_interface.hh"

/*****************************************************************************
RcsBuildUpTrailOff
Purpose:(Models RCS jets' build-up to full thrust and trail-off to zero thrust)
*****************************************************************************/
class RcsBuildUpTrailOff
{
 protected:
  RcsScaleFactorInterface& interface; /* (--) Structure for passing data to and
                                              from other scale factor models */
  RcsBuildUpTrailOffJetData* jet; /* (--) Pointer to array of per-jet data
                                          structures */
  const double & current_time; /* (s) Current time */

 public:
  bool   active; /* (--) Flag to enable the build-up/trail-off model */

  RcsBuildUpTrailOff( RcsScaleFactorInterface& interface_,
                      RcsBuildUpTrailOffJetData * const jet_,
                      const double& time);
  virtual ~RcsBuildUpTrailOff() {};
  virtual void build_up_trail_off_effects();

 private:
  RcsBuildUpTrailOff & operator = ( const RcsBuildUpTrailOff &);
  RcsBuildUpTrailOff( const RcsBuildUpTrailOff &);
};
#endif
