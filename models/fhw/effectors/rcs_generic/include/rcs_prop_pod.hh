/*******************************TRICK HEADER******************************
PURPOSE: (iThe RcsPropPod provides a convenient mechanism for grouping
  Rcs-jets together where they operate off the same tanks
  In the C-based model on which this object is based, the RCSGENERIC model
  had several instances of RCS_PPOD that needed instantiating; this object
  represents a very similar concept to RCS_PPOD.)

LIBRARY DEPENDENCY:
   ((../src/rcs_prop_pod.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2017) (Antares)
       (initial object-oriented implementation))
  )
**********************************************************************/


#ifndef RCS_GENERIC_PROP_POD_HH
#define RCS_GENERIC_PROP_POD_HH

#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/dynamics/mass/dynamic_mass/include/dynamic_mass_body_properties.hh"

/*****************************************************************************
RcsPodComponent
Purpose:(Represents each source of mass-draw-down, e.g. fuel and other gasses
         This class has 2 representations; the dyn-mass version has references
         to the Dyn-Mass model instances so that the component draw-down will
         be seen in a real mass draw-down.  The non-dynamic-mass version has
         dummy placeholders for those variables; the mass-consumed-step will
         still accumulate each step but no action will be taken.)
*****************************************************************************/
class RcsPodComponent{
 friend class RcsPropPod; // allows limiting access to parent-class only.
 protected:
  DynamicMassBodyPropertiesInterface fake_interface; /* (--)
        Dummy dyn-mass interfaces, used when there is no connection to
        an external dynamic-mass.  Components using this interface have
        unlimited propellant mass. */
  double * mass_consumed_step;  /* (kg)
      Mass consumed in this time-step. For monitoring only.*/
  double * consumable_mass; /* (kg)
      Consumable mass remaining. For consistency only.*/
  bool using_dyn_mass; /* (--)
      Indicates whether there is a true Dyn-Mass interface or whether
      the model is using the fake one provided here.
      Defaults to false (using fake) until one is provided. */

 public:
  std::vector<double> flow_rate_sf; /* (--)
       Array of propellant flow-rate SCALE FACTORS.
       The index of this vector represents the number of jets firing - 1.
       The values of the vector represents the scale-factor when that
       number of jets are firing.
       flow_rate_sf.at(0) is the scale factor when 1 jet is firing.
       flow_rate_sf.at(2) is the scale factor when 3 jets are firing.
       Use if RcsJetGroup::propc_use_isp = false and
              RcsGeneric::mult_jet_flag = true */
  double sum_consumption; /* (kg)
        Sum of all propellant consumed in this component.*/

  explicit RcsPodComponent( unsigned int max_num_jets_on);

  // external interface only, not used internally:
  void incr_mass_consumed_step(double incr) {*mass_consumed_step += incr;};

 protected:
  void set_dyn_mass_interface( DynamicMassBodyPropertiesInterface & interface);
  bool mass_available();
  void increment_mass_consumption( double consumption);

 private:
   // Don't declare copy constructor and operator to allow
   // compiler to implement default versions. The class simple enough
   // that the default versions are acceptable. It's necessary to have them
   // because RcsPropPod contains a vector of RcsPodComponent.
};

/*****************************************************************************
RcsPropPod
Purpose:(Propulsion Pod feeding some number of RCS jets.
         Has some number of associated jets and some number
         of propulsion components and other gases used by RCS (such as gas) to
         maintain pressure in tanks or gas used for pneumatic actutation of
         RCS valve)
*****************************************************************************/
class RcsPropPod{
 protected:
  double mass_epsilon; /* (kg) mass at which mass=0.0 is reasonable approx.*/
  double momentum_epsilon; /* (N*s)
         minimum equivalent momentum to register having a jet needed.*/
  const double  & time_step; /* (s) reference to the time-step in RcsGeneric. */
  const unsigned int max_num_jets_on; /* (--)
       The maximum number of jets that may be on at a time.  This should be the
       size of the thrust_factor vector.*/
  bool using_dyn_mass; /* (--)
       Defaults to false; is set to true if any of the dynamic-mass
       interfaces are assigned to real dynamic-masses. */
 public:
  /****** Controls ****/
  bool continue_thrust_after_depletion; /* (--)
       Flag used when the model is used to deplete mass, but it is not
       desirable for mass-depletion to end the thrust profile.
       Used only when "using_dyn_mass".
       Default: false, i.e. thrusters stop when they run out of propellant.)*/

  bool fail_on_depleted_mass; /* (--)
      Flag used to cause an automatic health-status transition to HealthFail if
      the string exhausts all of any component of its propellant (e.g. all of
      its fuel).  This flag has no effect if "continue_thrust_after_depletion"
      is true because the jets will continue thrusting under that situation.
      Default: false*/

  enum PodHealth{
    HealthUndefined = 0,
    HealthNominal = 1,
    HealthSuspect = 2,
    HealthFail = 3
  };
  PodHealth health; /* (--) Used for marking the health-status of a pod.*/

  /****** Inputs ******/
  double nominal_thrust ; /* (N)
       Thrust level used to determine the thrust factor array,
       Thrust_factor array is indexed according to equivalent number of
       nominal_thrust jets being fired.
       E.g. if all jets together produce 6 Ns per time-step and
            nominal_thrust = 2N, then there are 3 jets on for the purpose of
            the thrust-factor array index.
       Note that this is likely to be the same as c[0] for the blow-down model.
       Needed only if RcsGeneric::mult_jet_flag set */
  double pressure;    /* (N/m2)
       pressure used for blowdown model (from ext source) */

  std::vector<double> thrust_factor;/* (--)
       Array of jet thrust scale factors based on number of jets firing.
       The values of the vector represents the thrust-factor when thei
       number of jets firing = index+1.
       thrust_factor.at(0) is the thrust factor when 1 jet is firing.
       thrust_factor.at(2) is the thrust factor when 3 jets are firing.
       Needed only if RcsGeneric::mult_jet_flag set */

  // ******** Model components  **********
  std::vector<RcsPodComponent> components; /* (--)
       Each component represents some propulsion gas, such as fuel or
       pressurant.*/


  //  ********** Outputs  **********
  double sum_consumption; /* (kg)
        Sum of all propellant consumed for all components. */


  /****** Work space + Pointers + Structures ******/
  double equiv_momentum; /* (N*s)
       Working space to determine how many jets are on.  Equal to the time
       jets from each pod are on times the thrust for each jet */
  unsigned int num_jets_on;   /* (--) Number of jets firing from a prop pod */

  RcsPropPod( unsigned int max_num_jets_on,
              unsigned int num_components_,
              const double & time_step);
  virtual ~RcsPropPod(){};

  void set_dyn_mass_interface( unsigned int component_ix,
                DynamicMassBodyPropertiesInterface & dyn_mass_interface);
  void activate_dyn_mass();
  void deactivate_dyn_mass();
  void reset_cycle();
  bool mass_available();
  void increment_mass_consumption( std::vector<double> & jet_consumption);
  void compute_jets_on( bool mult_jet_flag );
  double get_flow_rate_scale_factor( const unsigned int component_index);
  double get_thrust_factor();
  unsigned int get_max_num_jets_on();
  void set_thrust_factor(unsigned int index, double value);
  bool is_healthy(){ return (health != HealthFail);}

 private:
   // Not implemented:
   RcsPropPod (const RcsPropPod& rhs);
   RcsPropPod & operator = (const RcsPropPod& rhs);;

};
#endif