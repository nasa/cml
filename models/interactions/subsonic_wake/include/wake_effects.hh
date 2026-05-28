/*******************************TRICK HEADER******************************
PURPOSE: (Definition of the wake effects.)

LIBRARY DEPENDENCY:
   ( (../src/reverse_flow.cc)
     (../src/prf_model.cc)
     (../src/wake_effects_base.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
   ((Bingquan Wang) (OSR) (Oct 2016) (Antares)
                (Library dependency warning fixing))
   ((Daniel Ghan) (OSR) (Jun 2020) (Antares) (Simplified user interface)))

NOTES:
     Body A - The wake-producing body
     Body B - The object inside the wake
**********************************************************************/
#ifndef WAKE_EFFECTS_HH
#define WAKE_EFFECTS_HH

#include <vector>
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/table_interp_cpp/include/simple_table_lookup.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_independent_variable.hh"
#include "cml/models/utilities/table_interp_cpp/include/generic_multi_input_table.hh"

#include "wake_params.hh"
#include "wake_bodies.hh"

/*****************************************************************************
WakeEffectsBase
Purpose:(Provides the common variable interface that is shared between
         Reverse-Flow and PRF models.)
Notes: (Abstract class.  Do not instantiate)
*****************************************************************************/
class WakeEffectsBase : public SubscriptionBase
{
 protected:
  // External references
  const WakeGeneratingBody & objectA;/* (--) reference to object causing wake. */
  const WakeFollowingBody & objectB; /* (--) reference to object in the wake. */
  const WakeParamsSub & params; /* (--)
       reference to the params associated with the particular sub-model that
       inherits this class.*/

 public:
  double  grid_offset;  /* (m)
       Offset of reverse-flow / PRF model origin from wake grid origin.
       Grid offset is specified as a distance along the body-frame x-axis,
       whereas grid origin is specified in the structural frame.*/
  double  force_toward_A; /* (N)
       Recirculation force on the following body acting toward the
       wake-generating body */

  bool data_loaded; /* (--)
       Have data been loaded into the look-up tables? This is a sanity check;
       the model will not initialize until the user sets this to true. */

 protected:
  bool in_region; /* (--)
       Flag to indicate following body is inside the recirculation zone.*/
  double  offset_trail_dist;  /* (m)
       Offset trailing distance for the reverse flow / PRF  model */
  double  offset_radial_dist; /* (m)
       Offset radial distance for the reverse flow / PRF  model  */
  double  eff_wake_vel; /* (m/s)
       Effective velocity in the recirculation zone due to
       reverse flow / PRF model.*/

 public:
  WakeEffectsBase( const WakeGeneratingBody & objectA_in,
                   const WakeFollowingBody & objectB_in,
                   const WakeParamsSub & params_in);

  virtual void update() = 0;
  bool get_in_region(){return in_region;};

 protected:
  virtual void deactivate() {
    force_toward_A = 0.0;
    SubscriptionBase::deactivate();
  };
  void generate_trail_and_radial_dist();

 private: // and undefined:
  WakeEffectsBase (const WakeEffectsBase& rhs);
  void operator = (const WakeEffectsBase& rhs);
};


/*****************************************************************************
WakeReverseFlow
Purpose:(Replaces old REV_FLOW struct, provides reverse-flow model variables.)
*****************************************************************************/
class WakeReverseFlow : public WakeEffectsBase
{
 public:
  double  Qrev; /* (--)
       Ratio of wake to free-stream dynamic pressure in the reverse flow model*/
  std::vector<double> offset_trail_dist_axis; /* (m)
       Offset-trailing-distance (independent) values for table look-up */
  std::vector<double> Qrev_table; /* (--) 1D look-up table for Qrev */

 protected:
  SimpleTableLookup Qrev_lookup; /* (--) Interpolation manager */

 public:
  WakeReverseFlow (const WakeGeneratingBody & objectA_in,
                   const WakeFollowingBody & objectB_in,
                   const WakeParamsSub & params_in);

  virtual void initialize();
  virtual void update() override;

 protected:
  virtual void deactivate() override;

 private: // and undefined:
  WakeReverseFlow (const WakeReverseFlow& rhs);
  void operator = (const WakeReverseFlow& rhs);
};


/*****************************************************************************
WakePrfModel
Purpose:(Replaces old PRF_MODEL; provides the Pressure Recovery Fraction (PRF)
         model variables.)
*****************************************************************************/
class WakePrfModel : public WakeEffectsBase
{
 protected: // External reference
  const double & main_body_diameter; /* (m)
       Reference diameter of the wake-generating body.*/

 public:
  double effective_area_sf; /* (--)
       A scaling factor to reduce the actual area to an effective area, used to
       generate the effective diameter.*/
  double PRF_mean;        /* (--)
       Mean value of interpolated PRF inside the wake region */
  double PRF;             /* (--)
       PRF including estimated uncertainty inside the wake region */
  double unc_factor;           /* (--)
       Uncertainty factor used to modify the interpolated mean PRF value to
       generate the actual PRF value.*/

  std::vector<double> mach_axis; /* (--)
       Mach-number (independent) axis values for table look-up */
  std::vector<double> alpha_axis; /* (rad)
       Angle-of-attack (independent) axis values for table look-up */
  std::vector<double> trail_diam_axis; /* (--)
       Trail-diameters (independent) axis values for table look-up */
  std::vector<double> effective_diam_axis; /* (--)
       Effective-diameter (independent) axis values for table look-up */
  std::vector<double> prf_table; /* (--) 4D look-up table for PRF_mean */

 protected:
  double trail_diameters; /* (--)
       Offset trailing distance in main body diameters */
  double effective_diam;  /* (m)
       Effective diameter of body inside the aft wake zone */

  TableLookupSet           table_set; /* (--) Interpolation manager */
  TableIndependentVariable indep_mach; /* (--)
       Mach-number independent variable for interpolator */
  TableIndependentVariable indep_alpha; /* (--)
       Angle-of-attack independent variable for interpolator */
  TableIndependentVariable indep_trail_diam; /* (--)
       Trail-diameter independent variable for interpolator */
  TableIndependentVariable indep_eff_diam; /* (--)
       Effective-diameter independent variable for interpolator */
  GenericMultiInputTable   prf_lookup; /* (--) PRF interpolator */

 public:
  WakePrfModel (const WakeGeneratingBody & objectA_in,
                const WakeFollowingBody & objectB_in,
                const WakeParams & params_in);

  virtual void initialize();
  virtual void update() override;

 protected:
  virtual void deactivate() override;

 private: // and undefined:
  WakePrfModel (const WakePrfModel& rhs);
  void operator = (const WakePrfModel& rhs);
};


/*****************************************************************************
WakeEffectsOut
Purpose:(Simple structure for passing model outputs to external models)
The intent here is that this class be instantiated in the external model that
   requires information about the wake at its location.  This external instance
   is passed to the SubsonicWake instance, which maintains a reference to the
   external model.
Then respective SubsonicWake model copies its data into the external model
  effects.prf_model.PRF = prf_model.PRF;
  effects.rev_flow.Qrev = rev_flow.Qrev;
(At the end of SubsonicWake::update())
Thus, this class serves as a really simple interface to facilitate the
  transfer of the important data without all the accompanying baggage.
*****************************************************************************/
class WakePrfOut{
 public:
  double PRF; /* (--) Copy of WakePrfModel::PRF for external use */
  bool in_region; /* (--) Copy of WakePrfModel::in_region for external use */
};
class WakeRevFlowOut {
 public:
  double Qrev; /* (--) Copy of WakeReverseFlow::Qrev for external use */
};
class WakeEffectsOut {
 public:
  WakePrfOut prf_model; /* (--) Output from PRF model */
  WakeRevFlowOut rev_flow; /* (--) Output from reverse-flow model */
};
#endif
