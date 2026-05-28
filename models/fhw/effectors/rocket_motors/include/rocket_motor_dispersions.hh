/*******************************************************************************
PURPOSE:
   (Provide a mechanism for dispersing the values associated with position
    and attitude of a rocket motor.
    This typically represents errors introduced in machining.)

LIBRARY DEPENDENCIES:
   ((../src/rocket_motor_dispersions.cc))

PROGRAMMERS:
   (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares) (initial)))
 ******************************************************************************/

#ifndef CML_SOLID_ROCKET_MOTOR_DISPERSIONS_HH
#define CML_SOLID_ROCKET_MOTOR_DISPERSIONS_HH

class RocketMotorDispersions {
public:
  double position_dispersion[3];  /* (m)   Dispersion in the position of the
                                           motor frame. */
  double motor_tolerance[3];      /* (rad) Dispersion in the attitude of the
                                           motor frame. */
  double tolerance_mag_threshold; /* (rad)
              Minimum operable magnitude of the vector motor_tolerance.
              If magnitude of motor_tolerance is less than this value,
              consequential dispersion in attitude will be ignored. */
  double T_dispersion_to_nominal[3][3]; /* (--)
              Transformation matrix from the actual motor frame to the
              nominal motor frame; accounts for manufacturing tolerances. */

  RocketMotorDispersions();
  virtual ~RocketMotorDispersions(){};

  void apply_dispersions( double position[3],
                          double T_parent_to_motor[3][3]);
private:
  // Not implemented:
  RocketMotorDispersions (const RocketMotorDispersions&);
  RocketMotorDispersions & operator = (const RocketMotorDispersions&);
};
#endif
