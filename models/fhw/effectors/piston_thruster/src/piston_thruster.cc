/*********************GENERIC PISTON THRUSTER MODEL HEADER *********************
PURPOSE:
   (To model the force and moments imparted on opposing sides of adjacent
    vehicles that were previosly connected and separated by the thrusters.)

ASSUMPTIONS AND LIMITATIONS:
  ((See piston_thruster_group.hh))

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

#include <cmath>   // sin, cos
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/piston_thruster.hh"

/*******************************************************************************
Constructors
*******************************************************************************/
PistonThruster::PistonThruster()
  :
  sideA(),
  sideB(),
  construction_complete(false)
{
  subscribe_name = "PistonThruster:";
}
/******************************************************************************/
PistonThruster::PistonThruster( const double * pos_CoM_A,
                                const double * pos_CoM_B)
  :
  sideA(),
  sideB(),
  construction_complete(false)
{
  subscribe_name = "PistonThruster:";
  loadCoM( pos_CoM_A, pos_CoM_B);
}
/******************************************************************************/
PistonThrusterVehicleSide::PistonThrusterVehicleSide()
  :
  param(),
  out(),
  T_local_to_struc(),
  trig_variable(),
  direction(),
  moment_arm(),
  position_struc(),
  position_CoM(nullptr)
{
  jeod::Matrix3x3::identity( T_local_to_struc);
  jeod::Vector3::initialize( position_local_frame_in_struc_frame);
}

/*******************************************************************************
loadCoM
Purpose:(Loads the respective centers of mass onto the two sides.
         This is called directly by the non-default constructor, but for users
         who need the default constructor -- e.g. for an array of thrusters --
         this method is provided for subsequent call.)
*******************************************************************************/
void
PistonThruster::loadCoM( const double * pos_CoM_A,
                         const double * pos_CoM_B)
{
  sideA.loadCoM( pos_CoM_A);
  sideB.loadCoM( pos_CoM_B);
  construction_complete = true;
}
/******************************************************************************/
void
PistonThrusterVehicleSide::loadCoM( const double * pos_CoM)
{
  if (pos_CoM == nullptr) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid argument for center-of-mass \n",
      "Cannot generate the torque without the center-of-mass location\n");
  }
  position_CoM = pos_CoM;
}

/*******************************************************************************
initialize
Purpose:(Initialize the individual thruster.)
Purpose:(Initialize the thruster group.)
*******************************************************************************/
void
PistonThruster::initialize()
{
  if (!enabled) {
    return;
  }

  if (!construction_complete) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid construction \n",
      "Construction omitted the center-of-mass location.\n"
      "Cannot generate the torque without the center-of-mass location\n");
  }

  sideA.initialize();
  sideB.initialize();
  SubscriptionBase::initialize();
}
/******************************************************************************/
void
PistonThrusterVehicleSide::initialize()
{
  trig_variable = std::cos( param.elevation);
  direction[0] =  std::sin( param.elevation);
  direction[1] =  trig_variable * std::sin( param.azimuth);
  direction[2] = -trig_variable * std::cos( param.azimuth);

  jeod::Vector3::transform( T_local_to_struc,
                            direction); // puts direction in struc frame

  jeod::Vector3::sum( param.position,
                      position_local_frame_in_struc_frame,
                      position_struc);
}

/*******************************************************************************
update
Purpose:(Main executive)
*******************************************************************************/
void
PistonThruster::update( double force_mag)
{
  if (!active) {
    return;
  }
  sideA.update( force_mag);
  sideB.update( force_mag);
}
/******************************************************************************/
void
PistonThrusterVehicleSide::update( double force_mag)
{
  jeod::Vector3::diff( position_struc,
                       position_CoM,
                       moment_arm);
  jeod::Vector3::scale( direction,
                        force_mag,
                        out.force);
  jeod::Vector3::cross( moment_arm,
                        out.force,
                        out.moment);
}

/*******************************************************************************
deactivate
Purpose:(Deactivates the model. Called internally via unsubscribe();
         see SubscriptionBase for more details.)
*******************************************************************************/
void
PistonThruster::deactivate()
{
  jeod::Vector3::initialize( sideA.out.force);
  jeod::Vector3::initialize( sideA.out.moment);
  jeod::Vector3::initialize( sideB.out.force);
  jeod::Vector3::initialize( sideB.out.moment);
  active = false;
}
