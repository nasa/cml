/*******************************TRICK HEADER******************************
PURPOSE: (The RcsPropPod provides a convenient mechanism for grouping
  Rcs-jets together where they operate off the same tanks
  In the C-based model on which this object is based, the RCSGENERIC model
  had several instances of RCS_PPOD that needed instantiating; this object
  represents a very similar concept to RCS_PPOD.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2017) (Antares)
       (initial object-oriented implementation))
  )
**********************************************************************/

#include "../include/rcs_prop_pod.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RcsPodComponent::RcsPodComponent(
   unsigned int max_num_jets_on)
   :
   fake_interface(),
   mass_consumed_step( &fake_interface.mass_consumed_step),
   consumable_mass(    &fake_interface.consumable_mass),
   using_dyn_mass(false),
   flow_rate_sf(max_num_jets_on, 0.0),
   sum_consumption(0.0)
{ }

/****************************************************************************/
RcsPropPod::RcsPropPod(
   unsigned int max_num_jets_on_,
   unsigned int num_components_,
   const double & time_step_)
   :
   mass_epsilon( 1.0e-12),
   momentum_epsilon( 1.0e-12),
   time_step(time_step_),
   max_num_jets_on(max_num_jets_on_),
   using_dyn_mass(false),
   continue_thrust_after_depletion(false),
   fail_on_depleted_mass(false),
   health(HealthUndefined),
   nominal_thrust(0.0),
   pressure(0.0),
   thrust_factor( max_num_jets_on, 0.0),
   components( num_components_, RcsPodComponent(max_num_jets_on_)),
   sum_consumption(0.0),
   equiv_momentum(0.0),
   num_jets_on(0)
{
  // This is not at all obvious.   Having the RcsPodComponent constructor
  // set the mass_consumed_step and consumable_mass pointers to the addresses
  // of the fake-interface components does not work because at that time,
  // the RcsPodComponent instance has not been added to the components vector,
  // so the address is a temporary one.  So we need to specify these pointers
  // now with the set_dyn_mass_interface method.
  // Another complication is that we need to use indexing rather than an
  // auto-iterator; the reason is not understood.  Use of
  //     for (auto component : components)
  //              component.set_dyn_mass_interface(component.fake_interface);
  // at this point still results in passing a temporary address for the
  // fake-interface so the pointers still go to the wrong place.
  for (size_t ii = 0; ii < components.size(); ++ii) {
    components[ii].set_dyn_mass_interface(components[ii].fake_interface);
  }
}

/*****************************************************************************
set_dyn_mass_interface
Purpose:(pushes the dynamic-mass-interface through to the specified component)
*****************************************************************************/
void
RcsPropPod::set_dyn_mass_interface(
      unsigned int component_ix,
      DynamicMassBodyPropertiesInterface & dyn_mass_interface)
{
  if (component_ix >= components.size()) {
    CMLMessage::error(
    __FILE__,__LINE__,"Assignment error\n",
    "Cannot assign a dyn-mass interface to component index ", component_ix, " because\n"
    "there are only ", components.size(), " components (so max index is ", components.size()-1, ").\n");
  }
  else {
    components.at(component_ix).set_dyn_mass_interface( dyn_mass_interface);
    using_dyn_mass = true;
  }
}
/****************************************************************************/
void
RcsPodComponent::set_dyn_mass_interface(
     DynamicMassBodyPropertiesInterface & dyn_mass_interface)
{
  if (&dyn_mass_interface != &fake_interface) {
    using_dyn_mass = true;
  }
  else {
    using_dyn_mass = false;
  }
  mass_consumed_step = &dyn_mass_interface.mass_consumed_step;
  consumable_mass = &dyn_mass_interface.consumable_mass;
}
/*****************************************************************************
activate_dyn_mass
Purpose:(Turns the dyn-mass capabilities on for all properly configured
         components.)
Note - by adding dyn-mass interfaces, the dyn-mass capabilities automatically
       activate.  This is more for reactivation following a forced
       deactivation.  More useful for sim config for testing edge cases
       than for nominal execution.
*****************************************************************************/
void
RcsPropPod::activate_dyn_mass()
{
  if (using_dyn_mass) {
    return;
  }
  for (std::vector<RcsPodComponent>::iterator it=components.begin();
       it != components.end();
       ++it) {
    if ((*it).mass_consumed_step != &(*it).fake_interface.mass_consumed_step) {
      using_dyn_mass = true;
      (*it).using_dyn_mass = true;
    }
  }
}
/*****************************************************************************
deactivate_dyn_mass
Purpose:(Turns the dyn-mass capabilities off for all properly configured
         components.)
*****************************************************************************/
void
RcsPropPod::deactivate_dyn_mass()
{
  if (using_dyn_mass) {
    using_dyn_mass = false;
    for (std::vector<RcsPodComponent>::iterator it=components.begin();
         it != components.end();
         ++it) {
      (*it).using_dyn_mass = false;
    }
  }
}



/*****************************************************************************
reset_cycle
Purpose:(Clears the cycle-specific data)
*****************************************************************************/
void
RcsPropPod::reset_cycle()
{
  equiv_momentum = 0.0;
  num_jets_on = 0;
}

/*****************************************************************************
mass_available
Purpose:(Tests whether mass is available on all components)
*****************************************************************************/
bool
RcsPropPod::mass_available()
{
  if (!using_dyn_mass) {
    return true; // mass is static; there is always mass available.
  }

  for (std::vector<RcsPodComponent>::iterator it=components.begin();
       it != components.end();
       ++it) {
    // if any component is out, return false.
    if ( !(*it).mass_available()) {
      // if the model is intended to be used such that mass depletes, but
      // mass-depletion does not prevent thrusting, turn off the dynamic-mass
      // at this point.  Mass will not deplete any further for any component.
      if (continue_thrust_after_depletion) {
        deactivate_dyn_mass();
        return true; // mass is not available but we don't care. OK to proceed.
      }
      if (fail_on_depleted_mass) {
        health = HealthFail;
      }
      return false; // mass is not available and this is a blocking condition
    }
  }
  return true; // no cases found where mass is not available, ok to proceed.
}
/****************************************************************************/
bool
RcsPodComponent::mass_available()
{
  // returns true if not using true dyn-mass or if there is mass remaining.
  return ( !using_dyn_mass || *consumable_mass > 0.0);
}


/*****************************************************************************
increment_mass_consumption
Purpose:(Adds mass consumption from each of the associated jets.)
*****************************************************************************/
void
RcsPropPod::increment_mass_consumption(
    std::vector<double> & jet_consumption) // vector of values per component
                                           // for a single jet
{
  if (jet_consumption.size() !=  components.size()) {
    CMLMessage::error(
      __FILE__,__LINE__," Incompatibility between jets and components.\n",
      "Mismatch on the number of propellant components. \n"
      "The jets have ", jet_consumption.size(), " components and the propulsion pod has ", components.size(), " components.\n"
      "Cannot accumulate propllant consumption from the jets unless these\n"
      "numbers are equal.\n");
  }

  for (unsigned int ii = 0; ii < components.size(); ++ii) {
    double incr_consumption = jet_consumption.at(ii);
    components.at(ii).increment_mass_consumption( incr_consumption);
    sum_consumption += incr_consumption;
  }
}
/****************************************************************************/
void
RcsPodComponent::increment_mass_consumption(
       double jet_consumption)
{
  if (using_dyn_mass) {
    *mass_consumed_step += jet_consumption;
  }
  sum_consumption += jet_consumption;
}


/*****************************************************************************
compute_jets_on
Purpose:(Computes the number of jets that are on for this pod)
*****************************************************************************/
void
RcsPropPod::compute_jets_on(
  bool mult_jet_flag)
{
  // if effect of multiple jets per fuel pod is to be considered
  if (mult_jet_flag) {
    // back out number of active jets: total momentum divided by momentum of
    // each jet.  NOTE: if one jet is on for less than half of time_step,
    // num_jets_on will round down to 0
    num_jets_on = static_cast<int>( 0.5 + equiv_momentum /
                                      (nominal_thrust * time_step));
  }
  else {
    // In this case, multiple jets DO NOT degrade thrust performance, and
    // a jet is at least partially on, so set num_jets_on to 1 so that
    // thrust_factor[0] and flow_rate_sf[0] will be used
    if ( equiv_momentum > momentum_epsilon) {
       num_jets_on = 1 ;
    }
  }

  // if jets are active, but the effective momentum is less than the steady
  // state momentum from one jet, force the num_jets_on value to be at
  // least 1
  if ( (num_jets_on == 0) && (equiv_momentum > momentum_epsilon)) {
    num_jets_on = 1;
  }

  /* protect for bad condition */
  if (num_jets_on > max_num_jets_on) {
    CMLMessage::error(
    __FILE__,__LINE__,"Computation error\n",
    "Number of jets attempting to simultaneously feed from\n"\
    "a prop pod (at least ", num_jets_on, ") exceeds number allowed (", max_num_jets_on, ")\n"
    "Overriding number of jets on to ", max_num_jets_on, ".\n");
    num_jets_on = max_num_jets_on;
  }
}



/*****************************************************************************
get_flow_rate_scale_factor
Purpose:(returns the scale-factor for the specified component in the pod
         after accounting for the number of jets that are currently firing.)
*****************************************************************************/
double
RcsPropPod::get_flow_rate_scale_factor(
   const unsigned int component_ix)
{
  if (num_jets_on == 0) {
    return 1.0;
  }
  if (component_ix >= components.size()) {
    CMLMessage::error(
    __FILE__,__LINE__,"Assignment error\n",
    "Cannot extract the flow-rate scale-factor from component index ", component_ix, "\n"
    "because there are only ", components.size(), " components (so max index is ", components.size()-1, ").\n");
    return 0.0;
  }

  return components.at(component_ix).flow_rate_sf[num_jets_on-1];
}


/*****************************************************************************
get_thrust_factor
Purpose:(Returns the currently used value from the thrust_factor vector)
*****************************************************************************/
double
RcsPropPod::get_thrust_factor()
{
  if (num_jets_on == 0) {
    return 1.0;
  }
  else {
    return thrust_factor.at(num_jets_on-1);
  }
}

/*****************************************************************************
get_max_num_jets_on
Purpose:(Returns the protected max_num_jets_on value)
*****************************************************************************/
unsigned int
RcsPropPod::get_max_num_jets_on()
{
  return max_num_jets_on;
}

/*****************************************************************************
set_thrust_factor
Purpose:(SWIG-friendly method to set values in the thrust_factor vector)
*****************************************************************************/
void
RcsPropPod::set_thrust_factor(
     unsigned int index,
     double value)
{
  if (index < thrust_factor.size()) {
    thrust_factor.at(index) = value;
  }
  else {
    CMLMessage::error(
    __FILE__,__LINE__,"Invalid assignment\n",
    "The thrust_factor vector is not sufficiently large to handle index ", index, ".\n");
  }
}