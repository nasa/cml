/*******************************************************************************
PURPOSE:
   (The top-level class representing the Generic Rcs System.)

REFERENCE:
   ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
    Rober Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
    (Trick code - rcs_generic.c by Willian Othon/LinCom/93))

LIBRARY DEPENDENCY:
   ((../src/rcs_generic.cc))

PROGRAMMERS:
   (((David Strack) (Lincom) (Feb 1997) (v1.0) (Initial Implementation))
    ((Gavin Mendeck) (LinCom) (Mar 1998) (v1.1) (revision))
    ((Olivier Rombout) (LinCom) (August 1999) (v1.1)
    (Added multiple RCS_GENERIC capability))
    ((Robert Gay) (Titan) (March 2003) (ATV) (changed random num gen))
    ((Olivier Rombout) (Odyssey) (March 2005) (RDLaa06518) (Unit testing +
                                  Sorted by Inputs vs. Work vs. Outputs +
                                  See 'OLR' in code))
    ((Remi Canton) (Odyssey) (Dec 2006) (clean-up))
    ((Gary Turner) (Odyssey) (Apr 2017) (conversion to C++)))
*******************************************************************************/

#ifndef RCS_GENERIC_HH
#define RCS_GENERIC_HH

#include <vector>
#include <random>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

// Forward declaration
class RcsPropPod;
class RcsJetGroup;
class RcsJet;

/*****************************************************************************
RcsGeneric
Purpose:( The manager-level object for the overall RcsGeneric model)
*****************************************************************************/
class RcsGeneric : public SubscriptionBase {
 protected:
  const double * cm; /* (--)
       pointer to the 3-array representing the position of center of mass
       of the vehicle in the structural frame. */
  const unsigned int num_propellant_components; /* (--)
       Number of propellant components: mono, bi, or tri prop with gas to
       maintain pressure or pneumatic valve actuator */

  std::vector<double> prop_loss_on;/* (kg)
       Mass of propellant component lost when a jet is turned on due to
       propellant being burned inefficiently or not at all or pneumatic
       actuators being used.  System-level setting for each propellant,
       used by all instances of RcsJet.*/
  std::vector<double> prop_loss_off;/* (kg)
       Mass of propellant component lost when a jet is turned off due to
       propellant being burned inefficiently or not at all or pneumatic
       actuators being used or purge gas.  System-level setting for each
       propellant, used by all instances of RcsJet. */

  // Pointers to the components of the RcsSystem (which is to be a sub-class of
  // RcsGeneric).
  std::vector<RcsJet *>  jets;         /* (--) Rcs jet parameters */
  std::vector<RcsPropPod *>  prop_pods;  /* (--) RCS propellant pod parameters */
  std::vector<RcsJetGroup *> groups;     /* (--) RCS jet groups */

  /****** Inputs ******/
  bool  mult_jet_flag;   /* (--)
       Flag if multiple jet effects on thrust and flow rates are to be enabled.
       SET AT INITIALIZATION ONLY. */
  bool  calc_flow_rate; /* (--)
       Flag indicating whether flow rate should be calculated based on thrust,
       specific impulse and mixture ratio.  Can only be used with mono or
       bi-propellants.  Will result in an override of any default settings
       for jet component-flow-rates.  INITIALIZATION ONLY. */
 public:
  /****** Inputs ******/
  bool  self_impingement; /* (--) Flag indicating whether impingement is on.*/

  bool  apply_thrust_factor_per_jet;  /* (--)
       On = per jet thrust factor is provided by external model or data.
       System level setting passed through to all jets.*/

  double imp_ref_center[3];/* (m)
       Point in Vehicle Structural frame about which the impingement torques
       are referenced.
       Used at runtime and only if self_impingement set. */
  enum InputForce {
    input_force_error  = 0,  // error state
    mag_and_uvec       = 1,  // force magnitude and unit vector
    vector             = 2   // force vector
  };
  InputForce input_force;   /* (--)
       Flag indicating method of user input for jet force */

  double time_step;    /* (s)
       The cycle rate of the rcs_gen module.
       Set here at initialization and subsequently accessed by RcsGroup,
       RcsJet and RcsPropPod.*/

  unsigned int seed; /* (--) Seed for random number generator */
  std::mt19937 generator; /* (--)
      Random number generator; using mt19937 rather than the default generator
      to avoid the problem of correlation with low seeds on uniform
      distributions.*/
  #ifndef SWIG // SWIG doesn't like templates
  std::normal_distribution<double> normal; /* (--) Normal distribution */
  std::uniform_real_distribution<double> uniform; /* (--) Uniform distribution*/
  #endif

  /****** Outputs ******/
  double force[3];       /* (N)  sum of the jet forces */
  double torque[3];      /* (N*m) sum of the jet torques */
  double total_imp_force[3]; /* (N)  sum of the jet self impingement forces */
  double total_imp_torque[3];/* (N*m) sum of the self impingement jet torques */
  std::vector<double> sum_component_consumptions;  /* (kg)
        sum of the propellant component consumptions.
        NOTE - this is public for the purposes of data-logging only.
        It is considered read-only.  In particular, the size of this vector
        must not be changed.*/
  double sum_consumption; /* (kg)  sum of the propellant consumptions */
  double sum_time;        /* (s)  sum of the jet on times */
  size_t num_jets;  /* (count) number of jets at initialization (output only).*/

  explicit RcsGeneric (const unsigned int num_propellant_components_);
  virtual ~RcsGeneric() {};

  virtual void initialize( double   time_step,
                           const double * center_of_mass);
  void update( const int  * rcs_command);
  void update( const bool * rcs_command);

  const double * get_cm() {return cm;};
  bool get_calc_flow_rate() {return calc_flow_rate;};
  double get_prop_loss_on(unsigned int ii) {return prop_loss_on.at(ii);}
  double get_prop_loss_off(unsigned int ii) {return prop_loss_off.at(ii);}

  void set_prop_loss_on(  unsigned int ix, double value);
  void set_prop_loss_off( unsigned int ix, double value);
  void set_mult_jet_flag (bool mult_jet_flag);
  void set_calc_flow_rate(bool calc_flow_rate);

 protected:
  void compute_force_and_fuel();
  void apply_self_impingement();
  bool update_part_I(const void *);
  void update_part_II();
  void check_mult_jet_flag_init();

 private:
   // Not implemented:
   RcsGeneric (const RcsGeneric& rhs);
   RcsGeneric & operator = (const RcsGeneric& rhs);
};
#endif
