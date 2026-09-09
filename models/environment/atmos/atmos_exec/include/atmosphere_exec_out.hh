/******************************** TRICK HEADER **********************************
PURPOSE:
   (Data structure for atmosphere executive output.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (Oct 2014) (Antares) (initial implementation))
   )
********************************************************************************/

#ifndef CML_ATMOSPHERE_EXEC_OUT_HH
#define CML_ATMOSPHERE_EXEC_OUT_HH

class AtmosExecOutput {
  public:
    double temperature{0.0};             /* (K)      Temperature */
    double pressure{0.0};                /* (N/m2)   Pressure */
    double density{0.0};                 /* (kg/m3)  Density */
    double speed_of_sound{0.0};          /* (m/s)    Speed of sound */
    double dynamic_viscosity{0.0};       /* (N*s/m2) Dynamic viscosity */
    double mean_free_path{0.0};          /* (m)      Mean free path */
    double wind_angle_blowing_from{0.0}; /* (rad)    Wind from angle wrt North */
    double wind_angle_blowing_to{0.0};   /* (rad)    Wind to angle wrt North */
    double wind_vmag{0.0};               /* (m/s)    Wind velocity magnitude */
    double wind_velocity_tc[3]{};        /* (m/s)    Wind velocity vector (Topocentric) */
    double wind_velocity_td[3]{};        /* (m/s)    Wind vel vector (Topodetic - NED) */
    double wind_velocity_eci[3]{};       /* (m/s)    Wind velocity vector (ECI) */
    double atmos_new_time{0.0};          /* (s)      Time at latest update */

    AtmosExecOutput() = default;
    virtual ~AtmosExecOutput() = default;
    AtmosExecOutput (const AtmosExecOutput&) = delete;
    AtmosExecOutput & operator = (const AtmosExecOutput&) = delete;

    void zero_outputs() { // Zeros everything except atmos_new_time
      temperature = pressure = density = speed_of_sound = dynamic_viscosity =
          mean_free_path = wind_angle_blowing_from = wind_angle_blowing_to =
          wind_vmag = 0.0;
      for (unsigned int i = 0; i < 3; i++) {
        wind_velocity_tc[i] = wind_velocity_td[i] = wind_velocity_eci[i] = 0.0;
      }
    }
};

#endif
