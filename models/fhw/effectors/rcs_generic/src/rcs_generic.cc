/*******************************TRICK HEADER******************************
PURPOSE: (The front-end of the rcs-generic model, which provides a
   functional model of a reaction control system with build up and trail
    off. The model accommodates: delay between command and actuation, thrust
    reduction based on fuel flow rates, self impingement, a user defined number
    of jets, errors in alignment and force, blow down effects of propellant module
    and fuel loss due to thrust, command on, and command off.
    The model calculates the forces and torques imparted on the vehicle by the
    jets and calculates the fuel loss in each tank.
    The model is partially based on rcs_orbiter by
    John Whynott/McDonnell Douglas/91,
    Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95 and partially based on
    rcs_generic by Willian Othon/LinCom/93)

REFERENCE:
    ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
      Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
     (Trick code - rcs_generic.c by Willian Othon/LinCom/93)
      CML code - last edited by remi Conton, 2006)

ASSUMPTIONS AND LIMITATIONS:
    ((Cycle rate is constant)
     (Delay time, Build up time, and Trail off time are all constant)
     (Jet force builds up linearly to a steady state value then trail off
       linearly)
     (If a jet is commanded on then off in less than the user input minimum
       value or the jet is commanded off then on in less than the user input
       minimum value, the jet will react as requested and an error will be
       flagged)
     (The jet can be commanded off during the build up and on during the trail
       off without error message unless the minimum on or minimum off times are
       violated)
     (ALL input jet force, torque and location vectors are referenced to the
       Vehicle Structural Frame)
     (Maximum number of jets-on connected to a single fuel tank is set in
       RcsPropPod and is pod-specific)
     (Number of liquid/gas components associated with an RCS jet is set in
       RcsPropPod and is pod-specific; this includes propellant components,
       pressure maintenance gasses and pneumatic gases)
     (The formulation that calculates thrust effects of multiple active jets
       on a propulsion system requires that all the jets have the same
       steady-state thrust magnitude. Systems with jets of multiple thrust can
       be modelled by using several calls to the RCS model with different
       initial conditions.)
     (The calculation of the flow rate based on thrust, specific impulse and
       mixture ratio assumes no more than 2 propellant components. If the
       blow-down model is also activated, it assumes only 1 propellant component)
     (Thrust magnitude and directions errors, if any, are added for each jet, and
       the seed can be randomized by Trick Monte-Carlo driver. However, the model
       does not include jet location errors: to do so, they may be generated with
       the Trick Monte-Carlo driver.)
     (See all other modules for additional assumptions))

PROGRAMMERS:
 (((David Strack) (Lincom) (Feb 1997) (v1.0) (Initial Implementation))
   ((Gavin Mendeck) (LinCom) (Mar 1998) (v1.1) (revisions))
   ((Bill Othon) (LinCom) (July 1998) (v1.1) (add pressure-based blowdown model))
   ((John Craft) (NASA/EG) (July 1998) (v1.1) (add pressure-based blowdown model))
   ((Robert Gay) (Titan) (March 2003) (ATV) (changed random num gen))
   ((David Hammen) (OSR) (Jan 2005) (RDLaa06518) (CMA update))
   ((Olivier Rombout) (Odyssey) (February 2005) (RDLaa06518)
          (INCOMPLETE Unit testing... + See 'OLR' throughout the code))
   ((Paul Gesting) (Odyssey) (03/06) (RDLaa08561)
          (Cleaned up conditional flag event))
   ((Remi Canton) (Odyssey) (12/06) (RDLaa09442)
          (Clean-up and modular reorganization)))
   ((Gary Turner) (OSR) (Apr 2017) (Antares) (Conversion to Object-oriented)))
**********************************************************************/

#include <cstddef> // NULL
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/rcs_generic.hh"
#include "../include/rcs_prop_pod.hh"
#include "../include/rcs_group.hh"
#include "../include/rcs_jet.hh"

/*****************************************************************************
Constructor
Purpose:()
*****************************************************************************/
RcsGeneric::RcsGeneric(
    const unsigned int num_propellant_components_)
  :
  cm (NULL),
  num_propellant_components(num_propellant_components_),
  prop_loss_on(num_propellant_components, 0.0),
  prop_loss_off(num_propellant_components, 0.0),
  jets(),
  prop_pods(),
  groups(),
  mult_jet_flag(false),
  calc_flow_rate(false),
  self_impingement(false),
  apply_thrust_factor_per_jet(false),
  imp_ref_center{0.0, 0.0, 0.0},
  input_force( input_force_error),
  time_step(0.0),
  seed(0),
  normal(0.0, 1.0),
  uniform(-1.0, 1.0),
  force{0.0, 0.0, 0.0},
  torque{0.0, 0.0, 0.0},
  total_imp_force{0.0, 0.0, 0.0},
  total_imp_torque{0.0, 0.0, 0.0},
  sum_component_consumptions(num_propellant_components, 0.0),
  sum_consumption(0.0),
  sum_time(0.0),
  num_jets(0)
{
  subscribe_name = "RcsGeneric:";
}

/*****************************************************************************
initialize
Purpose:(initializes the model)
PROGRAMMERS:
   (((David Strack) (Lincom) (Feb 1997) (v1.0) (Initial Implementation))
    ((Bill Othon) (Lincom) (July 1998) (v1.0) (Pressure-based blowdown model))
    ((John Craft) (NASA/EG) (July 1998) (v1.0) (Pressure-based blowdown model))
    ((E Lanoix) (LinCom) (Apr '02) (v1.0) (Fixed bug in error if statement))
    ((Robert Gay) (Titan) (March 2003) (ATV) (changed random num gen))
    ((Olivier Rombout) (Odyssey) (February 2005) (RDLaa06518) (INCOMPLETE Unit testing +
                                           See 'OLR' in code))
    ((Remi Canton) (Odyssey) (12/06) (RDLaa09442) (Clean-up and modular reorganization))
    ((Stephen Thrasher) (Draper) (06/07) (RDLaa09927) (Added test for time_step))
    ((Kurt McCall) (NASA) (06/07) (RDLaa09927) (Added time_step argument to entry point)))
*****************************************************************************/
void
RcsGeneric::initialize(
    double           time_step_in,   // time step for this model
    const double   * center_of_mass) // center-of-mass position

{
  if (!enabled) {
   return;
  }

  cm = center_of_mass;
  if (cm == NULL) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid array.\n",
    "The center of mass must be specified.\n");
  }

  time_step = time_step_in;
  if (time_step <= 0) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid time-step.\n",
    "Time step must be > 0.0.\n");
  }

  generator.seed(seed);

  for (unsigned int ii=0; ii< jets.size(); ii++) {
    if (jets.at(ii) == NULL) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "One of the jets pointers is NULL.\n"
        "This vector must be populated with valid pointers.\n");
    }
  }
  for (unsigned int ii=0; ii< prop_pods.size(); ii++) {
    if (prop_pods.at(ii) == NULL) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "One of the prop-pod pointers is NULL.\n"
        "This vector must be populated with valid pointers.\n");
    }
  }
  for (unsigned int ii=0; ii< groups.size(); ii++) {
    if (groups.at(ii) == NULL) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "One of the group pointers is NULL.\n"
        "This vector must be populated with valid pointers.\n");
    }
  }

  /************************************/
  /* Effect of multiple jets on tank? */
  /************************************/
  if (mult_jet_flag) {
    check_mult_jet_flag_init();
  }
  else {
    // no change in thrust due to number of jets active,
    // thrust factor[0] always 1.0
    for (unsigned int ii=0; ii < prop_pods.size(); ii++) {
      prop_pods[ii]->thrust_factor[0] =1.0;
      /* and flow rate scale factor[0] always 1.0 */
      for (unsigned int jj=0; jj< prop_pods[ii]->components.size(); jj++) {
         prop_pods[ii]->components[jj].flow_rate_sf[0] = 1.0;
       }
    }
  }

  //*************************************************************************
  // Validate the configuration for calculation of flow-rates
  //*************************************************************************
  if (calc_flow_rate && num_propellant_components > 2) {
    /* Protect against excessive # of propellant components */
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid configuration.\n",
    "'calc_flow_rate = true' assumes 1 or 2 prop components,\n"
    "but num_propellant_components = ", num_propellant_components, "\n");
  }

  //*****************************************************************************
  // initialize each of the groups
  //*****************************************************************************
  for (unsigned int ii=0; ii< groups.size() ; ii++) {
    groups.at(ii)->initialize(time_step);
  }

  //*****************************************************************************
  // Call the individual jet initialization routines:
  //*****************************************************************************
  num_jets = jets.size();
  for (unsigned int ii=0; ii< num_jets ; ii++) {
    jets.at(ii)->initialize();
  }

  SubscriptionBase::initialize();
}

/*****************************************************************************
check_mult_jet_flag_init
Purpose:(Checks for values needed if mult_jet_flag is set)
*****************************************************************************/
void
RcsGeneric::check_mult_jet_flag_init()
{
  /* for each propellant pod */
  for (unsigned int ii=0; ii< prop_pods.size(); ii++) {
    // make sure at least first thrust factor is on (non-zero)
    if ( MathUtils::is_near_equal( prop_pods.at(ii)->thrust_factor.at(0),
                                   0.0)) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "User has selected effects of multiple jets on thrust,\nbut no "
        "thrust factor was defined in thrust_factor[] array for pod ", ii, ".\n");
    }
    if ( MathUtils::is_near_equal( prop_pods.at(ii)->nominal_thrust, 0.0)) {
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid configuration\n",
        "User has selected effects of multiple jets on thrust,\n"
        "but no nominal thrust value was defined for pod ", ii, ".\n");
    }
  }
}

/*****************************************************************************
update
Purpose:(Main executive for RcsGeneric)
*****************************************************************************/
void
RcsGeneric::update(
   const int * rcs_command)
{
  if (!active ||
      !update_part_I(rcs_command)) {
    return;
  }

  // Loop through all jets.  Use index rather than iterator because the index is
  // also needed to extract the element of rcs_command
  for (unsigned int ii = 0; ii < jets.size(); ++ii) {
    jets[ii]->update(static_cast<bool>(rcs_command[ii]));
  }

  update_part_II();
}
/*****************************************************************************/
void
RcsGeneric::update(
   const bool * rcs_command)
{
  if (!active ||
      !update_part_I(rcs_command)) {
    return;
  }

  // Loop through all jets.  Use index rather than iterator because the index is
  // also needed to extract the element of rcs_command
  for (unsigned int ii = 0; ii < jets.size(); ++ii) {
    jets[ii]->update(rcs_command[ii]);
  }

  update_part_II();
}

/*****************************************************************************/
bool
RcsGeneric::update_part_I(
   const void * rcs_command)
{
  if (rcs_command == NULL) {
    CMLMessage::error(
    __FILE__,__LINE__,"Invalid arguments.\n",
    "RcsGeneric model was fed with a NULL command set.\n"
    "Model will not run.\nDisabling model.\n");
    disable();
    return false;
  }
  // Clear all pod data from last cycle
  for (std::vector<RcsPropPod*>::iterator pod_it = prop_pods.begin();
       pod_it != prop_pods.end();
       ++pod_it) {
    (**pod_it).reset_cycle();
  }
  return true;
}
/*****************************************************************************/
void
RcsGeneric::update_part_II()
{
  // Determine how many jets are on for calculation of the thrust factor */
  for (std::vector<RcsPropPod*>::iterator pod_it = prop_pods.begin();
       pod_it != prop_pods.end();
       ++pod_it) {
    (**pod_it).compute_jets_on(mult_jet_flag);
  }

  /*****************************************************/
  /* Calculate force and fuel consumption for each jet */
  /*****************************************************/
  compute_force_and_fuel();

  /**************************/
  /* Self-impingement model */
  /**************************/
  if( self_impingement ) {
     apply_self_impingement();
  }
}


/*****************************************************************************
compute_force_and_fuel
PURPOSE:
    ((To calculate the forces and torques imparted on the vehicle by the jets)
     (and calculate the propellant loss in each tank))
REFERENCE:
    ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
      Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
     (Trick code - rcs_generic.c by Willian Othon/LinCom/93)
     (CML code rcs_force_fuel_calc.c by Canton & Lanoix, OSR, 2006-7))
ASSUMPTIONS AND LIMITATIONS:
    ((1.All input jet force, torque and location vectors are referenced )
     ( to the Vehicle Structural Frame. )
     (2.All force and torque outputs are centered at the body CM,)
     ( but are referenced to the Vehicle Structural Frame.))
*****************************************************************************/
void
RcsGeneric::compute_force_and_fuel()
{
  // Clear model data from last cycle
  jeod::Vector3::initialize(force);
  jeod::Vector3::initialize(torque);

  for (std::vector<RcsJet*>::iterator jet_it = jets.begin();
       jet_it != jets.end();
       ++jet_it) {
    (**jet_it).compute_jet_forces();

    jeod::Vector3::incr( (**jet_it).force, force);
    jeod::Vector3::incr( (**jet_it).torque, torque);

    (**jet_it).compute_prop_consumption();
    for (unsigned int ii = 0; ii < num_propellant_components; ++ii) {
      double jet_component_step_consump =
                           (**jet_it).get_component_consumption(ii);
      sum_component_consumptions[ii] += jet_component_step_consump;
      sum_consumption += jet_component_step_consump;
    }
  }
  // TODO Turner April 2017
  // NOTE - at this point, if the pod has run out of propellant, it would be
  // possible to go back into the jets and scale the mass consumption of
  // those jets associated with this pod such that their consumption matches
  // that of the pod.  However, it is not clear that such a feature is
  // necessary or desirable, nor whether all components should be scaled
  // when only one component runs out.  This is left as a future area for
  // development.
}


/*****************************************************************************
apply_self_impingement
PURPOSE:
    ((To model the plume self-impingement forces and torques on the vehicle))
REFERENCE:
    ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
      Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
     (Trick code - rcs_generic.c by Willian Othon/LinCom/93))
     (CML code - rcs_self_impingement.c Canton(2006)
ASSUMPTIONS AND LIMITATIONS:
    ((1.Self-impingement forces and torques are about a user defined)
     (  impingement reference center)
     (2.The plume self-impingement model assumes that no vehicle surfaces are)
     (  moving, the jets do not gimbal and there is no error in the jet force)
     (  direction so that the area impinged upon is always the same. It further)
     (  assumes that the plume impingement forces and torques are directly)
     (  proportional to the magnitude of the jet force.))
*****************************************************************************/
void
RcsGeneric::apply_self_impingement()
{
  for (std::vector<RcsJet*>::iterator jet_it = jets.begin();
       jet_it != jets.end();
       ++jet_it) {
    (**jet_it).scale_self_impingement();
    jeod::Vector3::incr( (**jet_it).scaled_impingement_force,  total_imp_force);
    jeod::Vector3::incr( (**jet_it).scaled_impingement_torque, total_imp_torque);
  }

  /******************************************************************************/
  /* Transform forces and torques to CG location referenced to structural frame */
  /******************************************************************************/

  // Calculate CG to impingement reference location vector
  double delta_cg[3];
  jeod::Vector3::diff( imp_ref_center , cm, delta_cg );

  /* Calculate moment about CG due to impingement forces */
  jeod::Vector3::cross_incr(delta_cg , total_imp_force, total_imp_torque );

  /* Add self-impingement forces and torques into total */
  jeod::Vector3::incr(total_imp_force,  force );
  jeod::Vector3::incr(total_imp_torque, torque );
}


/*****************************************************************************
set_prop_loss_on
Purpose:(Allows modification of elements of the prop_loss_on vector)
*****************************************************************************/
void
RcsGeneric::set_prop_loss_on(
    unsigned int ix,
    double value)
{
  if (ix >= num_propellant_components) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid index error.\n",
      "The index passed in (", ix, ") to change the value of set_prop_loss_on is "
      "out of bounds\n.  Ignoring setting.\n");
  }
  else {
    prop_loss_on.at(ix) = value;
  }
}
/*****************************************************************************
set_prop_loss_off
Purpose:(Allows modification of elements of the prop_loss_off vector)
*****************************************************************************/
void
RcsGeneric::set_prop_loss_off(
    unsigned int ix,
    double value)
{
  if (ix >= num_propellant_components) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid index error.\n",
      "The index passed in (", ix, ") to change the value of set_prop_loss_off is "
      "out of bounds\n.  Ignoring setting.\n");
  }
  else {
    prop_loss_off.at(ix) = value;
  }
}
/*****************************************************************************
set_mult_jet_flag
Purpose:(sets the protected mult_jet_flag value)
*****************************************************************************/
void
RcsGeneric::set_mult_jet_flag(bool new_value)
{
  // If turning this on post-initialization, need to run the init-checks
  if( initialized && new_value) {
    check_mult_jet_flag_init();
  }
  mult_jet_flag = new_value;
}


/*****************************************************************************
set_calc_flow_rate
Purpose:(sets the protected calc_flow_rate value)
*****************************************************************************/
void
RcsGeneric::set_calc_flow_rate(bool new_value)
{
  if (initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid mid-sim adjustment.\n",
      "Because the calc_flow_value parameter is passed through to the jets\n"
      "at initialization, it cannot be adjusted mid-simulation.\n"
      "Adjustment FAILED.\n");
  }
  else {
    calc_flow_rate = new_value;
  }
}
