/*******************************************************************************
PURPOSE:
  (Provide class representing rocket nozzles to accompany
   RocketMotorMultiNozzle - a rocket motor with nozzles)

PROGRAMMERS:
  (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares)
       (initial, adapted from SolidRocketMotor2 and from las_prop_init.c))
   ((Daniel Ghan) (OSR) (Jun 2020) (Antares) (Separated nominal parameters from
            true parameters, simplified methods)))

******************************************************************************/
#define _USE_MATH_DEFINES
#include <cmath>  // M_PI, sin, cos
#include <cstddef> // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/rocket_motor_nozzle.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RocketMotorNozzle::RocketMotorNozzle()
  :
  azimuth(0.0),
  pitch(0.0),
  radius(0.0),
  height(0.0),
  sf(0.0),
  exit_area(0.0),
  azimuth_disp(0.0),
  pitch_disp(0.0),
  thrust_azm_disp(0.0),
  radius_disp(0.0),
  height_disp(0.0),
  sf_disp(0.0),
  sf_true(0.0),
  thrust_dir{0.0, 0.0, 0.0},
  nominal_thrust_dir{0.0, 0.0, 0.0},
  thrust{0.0, 0.0, 0.0},
  thrust_vac{0.0, 0.0, 0.0},
  azimuth_true(0.0),
  pitch_true(0.0),
  radius_true(0.0),
  height_true(0.0),
  position{0.0, 0.0, 0.0},
  thrust_mag(0.0),
  thrust_vac_mag(0.0),
  azimuth_thrust(0.0)
{}

/*****************************************************************************
initialize
Purpose:(Initialize the rocket motors nozzle class and generate position 
         values and apply dispersion values to them)
*****************************************************************************/
void
RocketMotorNozzle::initialize(
   double position_struc_to_motor_frame[3],
   double T_struc_to_motor_frame[3][3]) // pass-through
{
  if (T_struc_to_motor_frame == NULL)
  {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid initialization data.\n",
      "A NULL pointer was passed in for the transformation from structure\n"
      "to motor frame.\nCannot perform correct transformation.\n");
  }

  if (position_struc_to_motor_frame == NULL)
  {
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid initialization parameters\n",
      "RocketMotorNozzle requires a non-NULL position vector from the vehicle\n"
      "structure frame to the motor frame. \n");
  }

  if (exit_area < 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Initialization \n",
      "The exit area of a rocket nozzle cannot be < 0.\n"
      "Resetting the exit_area to 0.0 and continuing.\n"
      "Check the inputted exit area and make sure it is positive for all\n"
      "rocket motor nozzles. \n");
    exit_area = 0.0;
  }

  if (sf < 0.0) {
    CMLMessage::error(__FILE__, __LINE__, "Invalid nominal scale factor\n",
      "The nominal scale factor has a value of ", sf, ". It should be at least 0.0.\n"
      "Resetting the nominal scale factor to 0.0.\n");
    sf = 0.0;
  }

  // Apply dispersions
  sf_true        = sf      + sf_disp;
  azimuth_true   = azimuth + azimuth_disp;
  pitch_true     = pitch   + pitch_disp;
  radius_true    = radius  + radius_disp;
  height_true    = height  + height_disp;
  azimuth_thrust = azimuth_true + thrust_azm_disp + M_PI;

  if (sf_true < 0.0) {
    CMLMessage::error(__FILE__, __LINE__, "Invalid scale factor\n",
      "The dispersed scale factor has a value of ", sf_true, ". It must be at least\n"
      "0.0. This is the result of adding a negative dispersion to a\n"
      "nominal scale factor with a smaller absolute value. Resetting the\n"
      "dispersed scale factor to 0.0.\n");
    sf_true = 0.0;
  }

  // Calculate nominal thrust direction
  double sin_pitch = std::sin(pitch);
  nominal_thrust_dir[0] = -std::cos(pitch);
  nominal_thrust_dir[1] = -sin_pitch * std::sin(azimuth);
  nominal_thrust_dir[2] = -sin_pitch * std::cos(azimuth);

  // Transform into struct Coordinate System
  jeod::Vector3::transform_transpose( T_struc_to_motor_frame,
                                nominal_thrust_dir);

  // Calculate true thrust direction
  sin_pitch = std::sin(pitch_true);
  thrust_dir[0] = -std::cos(pitch_true);
  thrust_dir[1] =  sin_pitch * std::sin(azimuth_thrust);
  thrust_dir[2] =  sin_pitch * std::cos(azimuth_thrust);

  // Transform into struct Coordinate System
  jeod::Vector3::transform_transpose( T_struc_to_motor_frame,
                                thrust_dir);

  // position in motor frame:
  position[0] =  height_true;
  position[1] =  radius_true * std::sin(azimuth_true);
  position[2] =  radius_true * std::cos(azimuth_true);

  // Transform Nozzle Position Vector into struct Coordinate System
  jeod::Vector3::transform_transpose( T_struc_to_motor_frame,
                                position);
  jeod::Vector3::incr( position_struc_to_motor_frame,
                 position);
}

/*****************************************************************************
shutdown_nozzle
Purpose:(Shutdown the nozzle.)
*****************************************************************************/
void
RocketMotorNozzle::shutdown_nozzle()
{
  sf_true = 0.0;
  thrust_mag = 0.0;
  thrust_vac_mag = 0.0;
  jeod::Vector3::initialize(thrust);
  jeod::Vector3::initialize(thrust_vac);
}

/*****************************************************************************
compute_thrust_mag
Purpose:(Computes this nozzle thrust magnitude from its scale factor,
         the overall motor scale factor, and the overall motor thrust)
*****************************************************************************/
void
RocketMotorNozzle::compute_thrust_mag(
   double thrust_mag_per_sf)
{
  thrust_mag = thrust_vac_mag = sf_true * thrust_mag_per_sf;
}

/*****************************************************************************
modify_thrust_mag_atmos
Purpose:(Modifies the thrust magnitude to account for back-pressure from
         the atmosphere.)
*****************************************************************************/
void
RocketMotorNozzle::modify_thrust_mag_atmos(
   double atm_pressure)
{
  thrust_mag -= exit_area * atm_pressure;
  if (thrust_mag < 0.0) {
    thrust_mag = 0.0;
  }
}

/*****************************************************************************
compute_thrust_vec
Purpose:(Converts the thrust-mag into a vector.)
*****************************************************************************/
void
RocketMotorNozzle::compute_thrust_vec()
{
  jeod::Vector3::scale( thrust_dir, thrust_mag, thrust );
  jeod::Vector3::scale( thrust_dir, thrust_vac_mag, thrust_vac );
}