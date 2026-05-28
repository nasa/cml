/******************************* TRICK HEADER **********************************
PURPOSE:
    (To compute a new inertial to body transformation quaternion such that the chute
     is aligned with the wind relative velocity vector. This is ensure the chute has 
     as close to a zero angle of attack as is possible at start of inflation. 

ASSUMPTIONS AND LIMITATIONS:
    ((No Chute Failures))

PROGRAMMERS:
     ((Jeff Semrau) (Honeywell) (Oct-2013) (Initial Release, LMBP #3148)
*******************************************************************************/

/*
 * Include files for package
 */

#include <cmath>

#include "../include/parachute_exec.hh"

#include "trick/trick_math.h"

void parachute_attitude_reset (PARACHUTE_DATA&     chute, 
                               EOM_DATA  &           chute_eom)
                        
{

   double inert_uvec[3];
   double yaw, pitch, roll;
   double sin_roll_div_2, sin_pitch_div_2, sin_yaw_div_2;
   double cos_roll_div_2, cos_pitch_div_2, cos_yaw_div_2;
   double wrel_inert_vel[3];

   V_SUB(wrel_inert_vel, chute.input.pyld_inertial_vel, chute.wind_vel_inertial);

   //Compute Unit Vector of Payload Inertial Velocity
   V_NORM(inert_uvec,  wrel_inert_vel);
   V_INIT(chute_eom.output.body_rates);

   //Compute Euler Angles between chute body frame and inertial frame using
   //inertial velocity vector. These angles are to get the inertial velocity
   //expressed in only the chute X body axis.
   pitch = std::atan2(-inert_uvec[2],-inert_uvec[0] );
   yaw   = std::asin(inert_uvec[1]);
   roll  = 0.0; 

   sin_roll_div_2  = std::sin(roll/2);
   sin_pitch_div_2 = std::sin(pitch/2);
   sin_yaw_div_2   = std::sin(yaw/2);
   cos_roll_div_2  = std::cos(roll/2);
   cos_pitch_div_2 = std::cos(pitch/2);
   cos_yaw_div_2   = std::cos(yaw/2);

   //Compute New Quarternion of Chute
   chute_eom.work.quat_inertial_to_body[0] =
      sin_yaw_div_2 * sin_pitch_div_2 * sin_roll_div_2 + cos_yaw_div_2 * cos_pitch_div_2 * cos_roll_div_2;
   chute_eom.work.quat_inertial_to_body[1] =
      -sin_yaw_div_2 * sin_pitch_div_2 * cos_roll_div_2 + sin_roll_div_2 * cos_yaw_div_2 * cos_pitch_div_2;
   chute_eom.work.quat_inertial_to_body[2] =
      sin_yaw_div_2 * sin_roll_div_2 * cos_pitch_div_2 + sin_pitch_div_2 * cos_yaw_div_2 * cos_roll_div_2;
   chute_eom.work.quat_inertial_to_body[3] =
      sin_yaw_div_2 * cos_pitch_div_2 * cos_roll_div_2 - sin_pitch_div_2 * sin_roll_div_2 * cos_yaw_div_2;

}

