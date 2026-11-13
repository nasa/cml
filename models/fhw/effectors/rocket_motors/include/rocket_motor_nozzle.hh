/*******************************************************************************
PURPOSE:
   (Provide Nozzle class to accompany RocketMotor_MultiNozzle.)

LIBRARY DEPENDENCY:
   (../src/rocket_motor_nozzle.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (May 2015) (ANTARES) (C++ cleanup))
   ((Bingquan Wang) (OSR) (Oct 2016) (ANTARES) (Library dependency warning
                                                                        fixing))
   ((Brenton Caughron) (OSR) (Jan 2018) (Antares) (IV&V Code Review))
   ((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares)
           (refactor to accommodate new architecture))
   ((Daniel Ghan) (OSR) (Jun 2020) (Antares) (Separated nominal parameters from
            true parameters, simplified methods)))

******************************************************************************/

#ifndef CML_ROCKET_MOTOR_NOZZLE_HH
#define CML_ROCKET_MOTOR_NOZZLE_HH

class RocketMotorNozzle {
friend class RocketMotor_MultiNozzle;
public:
  // Nozzle Input Values
  // More information and figures on the definitions of the scale factor,
  // geometric parameters, and the coordinate system is in the documentation.
  double azimuth{0.0};   /* (rad) Nozzle Azimuth Angle in motor frame going
                             from the +z-axis to the +y-axis in the
                             motor-frame y-z plane */
  double pitch{0.0};     /* (rad) Nozzle Cant Angle in motor frame */
  double radius{0.0};    /* (m)   Distance to nozzle from motor-frame x-axis */
  double height{0.0};    /* (m)   Distance to nozzle from motor-frame y-z plane */
  double sf{0.0};        /* (--)  Nozzle scale factor */
  double exit_area{0.0}; /* (m2)  Exit area of nozzle; used only for computation of
                             atmospheric effect */

  // Dispersion Parameters (input)
  double azimuth_disp{0.0};    /* (rad) Nozzle misalignment azimuth angle, dPsi1,i */
  double pitch_disp{0.0};      /* (rad) Thrust misalignment pitch angle, dTheta,i */
  double thrust_azm_disp{0.0}; /* (rad) Thrust misalignment azimuth angle, dPsi2,i */
  double radius_disp{0.0};     /* (m)   Radius mispositioning error */
  double height_disp{0.0};     /* (m)   Height mispositioning error */
  double sf_disp{0.0};         /* (--)  Dispersion on nozzle scale factor */

  double sf_true{0.0};         /* (--)  Dispersed and scaled scale factor--public so
                                        the user can adjust it after initialization*/
  double thrust_dir[3]{};      /* (--)  Unit vector of nozzle thrust in structural
                                        frame, calculated from the azimuth, pitch,
                                        and structure-to-frame transformation. */
  double nominal_thrust_dir[3]{}; /* (--)
                                     Like thrust_dir but calculated using nominal
                                     azimuth and pitch rather than true azimuth
                                     and pitch. */
  double thrust[3]{};     /* (N)   Thrust vector of nozzle */
  double thrust_vac[3]{}; /* (N)   Vacuum Thrust vector of nozzle */

protected:
  double azimuth_true{0.0};   /* (rad) Dispersed azimuth */
  double pitch_true{0.0};     /* (rad) Dispersed pitch */
  double radius_true{0.0};    /* (m)   Dispersed radius */
  double height_true{0.0};    /* (m)   Dispersed height */
  double position[3]{};       /* (m)   Nozzle positions in structural frame
                                       calculated from the azimuth and radius,
                                       assuming that all the nozzle positions are
                                       coplanar in the motor-frame y-z plane. */
  double thrust_mag{0.0};     /* (N)   Thrust magnitude of nozzles */
  double thrust_vac_mag{0.0}; /* (N)   Vacuum Thrust magnitude of nozzle */

  // Internal members
  double azimuth_thrust{0.0}; /* (rad) A value derived from azimuth and its
                                       dispersions, this specifies the orientation
                                       of the thrust vector. */

public:
  RocketMotorNozzle() = default;
  virtual ~RocketMotorNozzle() = default;
  RocketMotorNozzle (const RocketMotorNozzle& rhs) = delete;
  RocketMotorNozzle & operator = (const RocketMotorNozzle& rhs) = delete;

  void initialize( double position_struc_to_motor_frame[3],
                   double T_struc_to_motor_frame[3][3]);
  void shutdown_nozzle();

protected:
  // Used by RocketMotor_MultiNozzle
  void compute_thrust_mag( double thrust_mag_per_sf);
  void modify_thrust_mag_atmos (double atm_pressure);
  void compute_thrust_vec();
};
#endif