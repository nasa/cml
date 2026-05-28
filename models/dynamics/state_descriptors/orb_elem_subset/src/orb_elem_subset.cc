/********************************* TRICK HEADER *******************************
PURPOSE:
   (Parameter definition for subset of orbital elements.)
REFERENCES:
    ((JEOD Orbital Elements model)
     ((Vallado, D.) (Fundamentals of Astrodynamics and Applications)))
ASSUMPTIONS AND LIMITATIONS:
   ((If full orbital elements are needed, use JEOD's orbital elements model)
    (This class is intended to be used to support the impact-point and
    apsides-computation models and provides only enough to do so))
PROGRAMMERS:
    ((Gary Turner) (OSR) (Oct 2014) (Antares))
*******************************************************************************/

#include <cmath>
#include "jeod/models/environment/planet/include/planet.hh"

#include "../include/orb_elem_subset.hh"

/*****************************************************************************
constructor
*****************************************************************************/
OrbElemSubset::OrbElemSubset(const jeod::RefFrameTrans & inrtl_state_in,
                             const jeod::Planet & planet_in,
                             const double & planet_mu)
  :
  use_pfix_z_values(false),
  position{0.0, 0.0, 0.0},
  position_unit{0.0, 0.0, 0.0},
  position_mag(0.0),
  velocity{0.0, 0.0, 0.0},
  vel_vertical{0.0, 0.0, 0.0},
  vel_horizontal_unit{0.0, 0.0, 0.0},
  vel_vertical_scalar(0.0),
  semi_major_axis(0.0),
  semi_parameter(0.0),
  ecc_mag(0.0),
  ecc_anomaly(0.0),
  true_anomaly(0.0),
  arg_periapsis(0.0),
  mean_motion(0.0),
  orbit_type(UNDEFINED),
  specific_energy(0.0),
  tol_energy(1.0E-6),
  ang_momentum{0.0, 0.0, 0.0},
  node_line{0.0, 0.0, 0.0},
  cos_true_anomaly(0.0),
  node_angle(0.0),
  sinI_sinL(0.0),
  sinI_cosL(0.0),
  sin_sq_I(0.0),
  e_sinE(0.0),
  e_cosE(0.0),
  j2(0.0010836), // Earth
  j2_r_eq_2(0.0),
  j2_correction(0.0),
  j2_delta(0.0),
  inertial_state(inrtl_state_in),
  planet(planet_in),
  mu(planet_mu),
  polar_axis_inrtl{0.0, 0.0, 0.0}
{}


/*****************************************************************************
initialize
Purpose:(Sets planet-specific values)
*****************************************************************************/
void
OrbElemSubset::initialize()
{
  // Set local values
  j2_r_eq_2 = j2 * planet.r_eq * planet.r_eq;
}

/*****************************************************************************
update
Purpose:(Primary call)
*****************************************************************************/
void
OrbElemSubset::update()
{
  // Store the current position.
  // Generate the radius and position unit vector from it.
  jeod::Vector3::copy(inertial_state.position, position);
  position_mag = jeod::Vector3::vmag(position);
  jeod::Vector3::scale(position, (1.0/position_mag), position_unit);

  // Store the current velocity.
  // Decompose it into horizontal and vertical.
  // Keep the scalar (+up/-down) of the vertical component.
  jeod::Vector3::copy(inertial_state.velocity, velocity);
  vel_vertical_scalar = jeod::Vector3::dot(velocity, position_unit);
  jeod::Vector3::scale(position_unit, vel_vertical_scalar, vel_vertical);
  jeod::Vector3::diff(velocity, vel_vertical, vel_horizontal_unit); // temporary
  jeod::Vector3::normalize(vel_horizontal_unit);

  // semi-major axis
  // spec. energy = -mu/r + v^2 /2 = -mu / (2 * a)  => a= ...
  specific_energy = -mu/position_mag + jeod::Vector3::dot(velocity, velocity)/2.0;
  if (specific_energy <= tol_energy && specific_energy >= -tol_energy) {
    // Energy is near-zero; reset to prevent semi-major axis overflow 
    specific_energy = -tol_energy; 
    orbit_type = PARABOLA;
  } 
  else if (specific_energy < 0.0) {
    orbit_type = ELLIPSE;
  } 
  else if (specific_energy > 0.0) {
    orbit_type = HYPERBOLA;
  }
  semi_major_axis = -mu / (2.0 * specific_energy);
  
  // angular momentum
  jeod::Vector3::cross( position, velocity, ang_momentum);

  // semi-latus rectum / semi-parameter
  semi_parameter = jeod::Vector3::dot(ang_momentum, ang_momentum) / mu;

  if (use_pfix_z_values) {
    // polar-axis is [0,0,1] in pfix, transform to inertial:
    // The bottom row of T inrtl->pfix provides the pfix z-axis in inertial
    // So just copy it.
    jeod::Vector3::copy(planet.pfix.state.rot.T_parent_this[2], 
                  polar_axis_inrtl);
    sinI_sinL = jeod::Vector3::dot(polar_axis_inrtl, position_unit);
    sinI_cosL = jeod::Vector3::dot(polar_axis_inrtl, vel_horizontal_unit);
  }
  else { // assume polar axis is inertial z-axis (default)
    sinI_sinL = position_unit[2];
    sinI_cosL = vel_horizontal_unit[2];
  }
  
  sin_sq_I  = sinI_sinL * sinI_sinL + sinI_cosL * sinI_cosL;
}

/*****************************************************************************
compute_eccentricities
Purpose:(Computes eccentricity magnitude and eccentric_anomaly)
Assumption:( SMA > 0)
*****************************************************************************/
void
OrbElemSubset::compute_eccentricities()
{
  if (semi_major_axis <= 0) {
    return;
  }
  
  e_sinE = position_mag * vel_vertical_scalar / std::sqrt(mu * semi_major_axis);
  e_cosE = 1.0 - position_mag / semi_major_axis;

  ecc_mag = std::sqrt( e_sinE * e_sinE +  e_cosE * e_cosE);

  if (ecc_mag < 1.0) {
    ecc_anomaly = std::atan2( e_sinE, e_cosE);
  }
}

/*****************************************************************************
compute_true_anomaly
Purpose:(computes true anomaly)
*****************************************************************************/
void
OrbElemSubset::compute_true_anomaly()
{
  cos_true_anomaly = (semi_parameter - position_mag) / (ecc_mag * position_mag);

  if (cos_true_anomaly < 1.0) {
    if (cos_true_anomaly > -1.0) {
      true_anomaly = std::acos(cos_true_anomaly);
    } else {
      cos_true_anomaly = -1.0;
      true_anomaly = M_PI;
    }
  } else {
    cos_true_anomaly = 1.0;
    true_anomaly = 0.0;
  }

  if (vel_vertical_scalar < 0) {
    true_anomaly *= -1.0;
  }
}

/*****************************************************************************
compute_arg_periapsis
Purpose:(Computes the node angle and argument of periapsis)
Requires: true_anomaly
*****************************************************************************/
void
OrbElemSubset::compute_arg_periapsis()
{
  // node_angle is the angle measured from the ascending node around the orbit
  // argument of periapsis is the angle from the ascending node to the
  // periapsis vector (from where true anomaly is measured)
  if (sin_sq_I > 1.0E-12) {
    node_angle = std::atan2(sinI_sinL, sinI_cosL);
    arg_periapsis = node_angle - true_anomaly;
  }
  else {
    node_angle = 0.0;
    arg_periapsis = 0.0;
  }
}

/*****************************************************************************
compute_line_of_nodes
Purpose:(Computes the line-of-nodes vector)
*****************************************************************************/
void
OrbElemSubset::compute_line_of_nodes()
{
  // the line of nodes vector is oriented along the vector produced by the cross
  // product of the inertial z-axis with the angular momentum vector.  Since the
  // angular momentum vector is already expressed in inertial, this is trivial.
  node_line[0] = -ang_momentum[1];
  node_line[1] = ang_momentum[0];
  node_line[2] = 0.0;
  jeod::Vector3::normalize(node_line);
}

/*****************************************************************************
generate_cartesian_inertial
Purpose:(Generates inertial position vector)
Requires position_mag, node_line
*****************************************************************************/
void
OrbElemSubset::generate_cartesian_inertial()
{
  compute_line_of_nodes();


  // O - Omega, the argument of ascending node
  double cosO = node_line[0];
  double sinO = node_line[1];

  // I - angle of inclination.
  double sinI = std::sqrt(sin_sq_I);
  double cosI = std::sqrt(1.0 - sin_sq_I);

  // W - omega, argument of periapsis
  double cosW = std::cos(arg_periapsis);
  double sinW = std::sin(arg_periapsis);

  // Positions in PQW frame ( x=periapsis, z=ang_mom, y completes)
  double x_pqw = position_mag * cos_true_anomaly;
  double y_pqw = position_mag * std::sin(true_anomaly);

  // Transform to inertial.  See JEOD Orbital Elements for unabridged version.
  position[0] = (cosO * cosW - sinO * sinW * cosI) * x_pqw -
                (cosO * sinW + sinO * cosW * cosI) * y_pqw;
  position[1] = (sinO * cosW + cosO * sinW * cosI) * x_pqw -
                (sinO * sinW - cosO * cosW * cosI) * y_pqw;
  position[2] = (sinW * sinI) * x_pqw +
                (cosW * sinI) * y_pqw;
}

/*****************************************************************************
compute_mean_motion
Purpose:(Computes the mean motion)
*****************************************************************************/
void
OrbElemSubset::compute_mean_motion()
{
  if (semi_major_axis > 0) {
    mean_motion = std::sqrt( mu / semi_major_axis) / semi_major_axis;
  }
}

/*****************************************************************************
correct_for_j2
Purpose:(adjust parameters to account for J2)
*****************************************************************************/
void
OrbElemSubset::correct_for_j2()
{
  j2_correction = j2_r_eq_2 / (4.0 * semi_parameter);
  j2_delta = j2_correction * (sinI_cosL * sinI_cosL -
                      sinI_sinL * sinI_sinL);

  // calculate mean semimajor axis and inverse of the mean orbital rate
  semi_major_axis -= (((j2_r_eq_2 * semi_major_axis * semi_major_axis) /
                       (position_mag * position_mag * position_mag)) *
                      (1.0 - 3.0 * sinI_sinL * sinI_sinL));

  compute_mean_motion();

  /* compute adjusted orbital radus, vertical velocity, and semimajor axis */
  position_mag -= j2_delta;
  vel_vertical_scalar += (4.0 * j2_correction * sinI_sinL*sinI_cosL * mean_motion);
  semi_major_axis -= (j2_correction * (2.0 - 3.0 * sin_sq_I));
}
