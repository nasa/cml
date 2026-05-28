/*********************GENERIC PISTON THRUSTER MODEL HEADER *********************
PURPOSE:
  (To model the force and moments imparted on both parts of a vehicle
   during its separation when a piston thruster or a group of them fires.)

ASSUMPTIONS AND LIMITATIONS:
  (See header file)

PROGRAMMERS:
  (((Jeff Semrau) (Honeywell) (Oct-2011) (Initial Implementation))
   ((Mark McPherson) (NASA/DM42) (Oct-2011) (Integration into ANTARES))
   ((Mark McPherson) (NASA/DM42) (May-2012) (lmbp 1650)
                                   (New Thruster Load Data Table Lookup))
   ((Mark McPherson) (NASA/DM42) (June-2013) (AGDL200001040)
                                     (Updates to allow jettison delta-v))
   ((Gary Turner) (OSR) (Apr 2016) (Antares-CML)
                            (genericized the model developed for Antares))
   ((Brent Caughron) (OSR) (June 2017) (Antares) (IV&V code review clean up)))
*******************************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/piston_thruster_group.hh"

/*******************************************************************************
Constructors
*******************************************************************************/
PistonThrusterGroupInputs::PistonThrusterGroupInputs( const double & time_in,
                                                      double & thrust_in)
  :
  duration(0.0),
  deltaV(0.0),
  load_mass(0.0),
  load_profile(1.0),
  table_set(),
  load_pct_table( load_profile),
  time_table( time_in),
  thrust_table( thrust_in)
{
  table_set.subscribe();
}
/******************************************************************************/
PistonThrusterGroup::PistonThrusterGroup( const double & time_in)
  :
  input( time_in,
         force_mag),
  param(),
  out_A(),
  out_B(),
  force_mag(0.0),
  start_time(0.0),
  time( time_in)
{
  subscribe_name = "PistonThrusterGroup:";
}

/*******************************************************************************
add_piston_thruster
Purpose:(Adds a piston thruster instance to the group.)
*******************************************************************************/
void
PistonThrusterGroup::add_piston_thruster( PistonThruster & new_thruster)
{
  if (active) {
    CMLMessage::error(
      __FILE__,__LINE__, "Invalid thruster addition\n",
      "Attempted to add a thruster to the thruster group during the active phase."
      "\nAttempt failed.\n");
    return;
  }

  for (std::list<PistonThruster *>::iterator it =  piston_thrusters.begin();
                                             it != piston_thrusters.end();
                                           ++it) {
    if ( (*it) == &new_thruster) {
      CMLMessage::error(
        __FILE__,__LINE__, "Invalid thruster addition.\n",
        "Attempted to add a thruster instance that has already been added.\n"
        "\nAttempt failed.\n");
      return;
    }
  }

  piston_thrusters.push_back( &new_thruster);
  if (initialized) {
    new_thruster.initialize();
  }
}

/*******************************************************************************
initialize
Purpose:(Initialize the individual thruster.)
Purpose:(Initialize the thruster group.)
*******************************************************************************/
void
PistonThrusterGroup::initialize()
{
  if (!enabled) {
    return;
  }

  for (std::list<PistonThruster *>::iterator it =  piston_thrusters.begin();
                                             it != piston_thrusters.end();
                                           ++it) {
    (*it)->initialize();
  }

  SubscriptionBase::initialize();
}

/*******************************************************************************
activate
Purpose:(Activates the group, makes the thrusters start.)
*******************************************************************************/
void
PistonThrusterGroup::activate()
{
  if (param.mode == PistonThrusterGroupParams::ReferenceDeltaV) {
    if (MathUtils::is_near_equal(input.duration, 0.0)) {
      CMLMessage::error(
        __FILE__,__LINE__, "Invalid thruster inputs\n",
        "Attempted to start a thruster group with a fixed deltaV over zero time."
        "\n Requires an infinite force.\n"
        "Recommendation: use the impulsive-detach mechanism instead.\n"
        "Thrusters FAILED.\n");
      return;
    }
    if (MathUtils::is_near_equal(input.load_mass, 0.0) ||
        MathUtils::is_near_equal(input.deltaV, 0.0)) {
      CMLMessage::error(
        __FILE__,__LINE__, "Invalid thruster inputs\n",
        "Attempted to start a thruster group with a fixed deltaV with no "
        "required impulse.\n"
        "Thrusters do not need to fire to achieve zero impulse.\n"
        "Reference mass = ", input.load_mass, "\n"
        "Reference delta-v = ", input.deltaV, " \n");
      return;
    }
    // Compute the force required to generate the specified delta-v on
    // the load mass, assuming a square wave impulse
    force_mag = input.load_mass * input.deltaV / input.duration;
  }

  start_time = time;
  // activate each of the individual thrusters.
  for (std::list<PistonThruster *>::iterator it =  piston_thrusters.begin();
                                             it != piston_thrusters.end();
                                           ++it) {
    (*it)->subscribe();
  }
  active = true;
  update();
}

/*******************************************************************************
update
Purpose:(Main executive.)
*******************************************************************************/
void
PistonThrusterGroup::update()
{
  if (!active) {
    return;
  }

  // Compute the elapsed time since the PECs were fired
  double elapsed_time = time - start_time;

  // End if been active too long
  if (elapsed_time > param.max_time){
    deactivate();
    return;
  }

  switch (param.mode) {
  // Compute the force per thruster given a delta-v
  case PistonThrusterGroupParams::ReferenceDeltaV:
    if (elapsed_time > input.duration){
      deactivate();
      return;
    }
    break;
  // Use a 2D Table Lookup to Compute Current Force
  case PistonThrusterGroupParams::TimeTableLookup:
    input.table_set.update();
    break;
  default:
    CMLMessage::error(
      __FILE__,__LINE__, "Unsupported mode\n",
      "Attempted to execute a thruster group with an invalid/unsupported\n"
      "force-generation method.  Turning thrusters off.\n");
    deactivate();
    break;
  }

  // Sum up forces and moments
  jeod::Vector3::initialize( out_A.force);
  jeod::Vector3::initialize( out_A.moment);
  jeod::Vector3::initialize( out_B.force);
  jeod::Vector3::initialize( out_B.moment);

  for (std::list<PistonThruster *>::iterator it =  piston_thrusters.begin();
                                             it != piston_thrusters.end();
                                           ++it) {
    (*it)->update(force_mag);
    jeod::Vector3::incr( (*it)->sideA.out.force , out_A.force  );
    jeod::Vector3::incr( (*it)->sideA.out.moment, out_A.moment );
    jeod::Vector3::incr( (*it)->sideB.out.force , out_B.force  );
    jeod::Vector3::incr( (*it)->sideB.out.moment, out_B.moment );
  }
}

/*******************************************************************************
set_mode
Purpose:(Sets the mode.)
*******************************************************************************/
void
PistonThrusterGroup::set_mode( PistonThrusterGroupParams::ForceMethod new_mode)
{
  if (active) {
    CMLMessage::error(
      __FILE__,__LINE__, "Invalid mode-change request\n",
      "Cannot adjust mode while thrusters are active.\n"
      "Mode remains at ", param.mode, "\n");
  }
  else {
    param.mode = new_mode;
  }
}

/*******************************************************************************
deactivate
Purpose:(Deactivation of thruster group and outputs.)
*******************************************************************************/
void
PistonThrusterGroup::deactivate()
{
  jeod::Vector3::initialize( out_A.force);
  jeod::Vector3::initialize( out_A.moment);
  jeod::Vector3::initialize( out_B.force);
  jeod::Vector3::initialize( out_B.moment);
  // deactivate each of the individual thrusters.
  for (std::list<PistonThruster *>::iterator it =  piston_thrusters.begin();
                                             it != piston_thrusters.end();
                                           ++it) {
    (*it)->unsubscribe();
  }
  active = false;
}