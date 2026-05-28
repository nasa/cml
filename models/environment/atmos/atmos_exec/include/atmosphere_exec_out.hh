/******************************** TRICK HEADER **********************************
PURPOSE:
   (Data structure for atmosphere executive output.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (Oct 2014) (Antares) (initial implementation))
   )
********************************************************************************/

#ifndef ATMOS_EXEC_OUT_HH
#define ATMOS_EXEC_OUT_HH

class AtmosExecOutput {
  public:
    double temperature;              /* (K)      Temperature */
    double pressure;                 /* (N/m2)   Pressure */
    double density;                  /* (kg/m3)  Density */
    double speed_of_sound;           /* (m/s)    Speed of sound */
    double dynamic_viscosity;        /* (N*s/m2) Dynamic viscosity */
    double mean_free_path;           /* (m)      Mean free path */
    double wind_angle_blowing_from;  /* (rad)    Wind from angle wrt North */
    double wind_angle_blowing_to;    /* (rad)    Wind to angle wrt North */
    double wind_vmag;                /* (m/s)    Wind velocity magnitude */
    double wind_velocity_tc[3];      /* (m/s)    Wind velocity vector (Topocentric) */
    double wind_velocity_td[3];      /* (m/s)    Wind vel vector (Topodetic - NED) */
    double wind_velocity_eci[3];     /* (m/s)    Wind velocity vector (ECI) */
    double atmos_new_time;           /* (s)      Time at latest update */

    AtmosExecOutput()
      :
      temperature(0.0),
      pressure(0.0),
      density(0.0),
      speed_of_sound(0.0),
      dynamic_viscosity(0.0),
      mean_free_path(0.0),
      wind_angle_blowing_from(0.0),
      wind_angle_blowing_to(0.0),
      wind_vmag(0.0),
      wind_velocity_tc(),
      wind_velocity_td(),
      wind_velocity_eci(),
      atmos_new_time(0.0)
    {};
    virtual ~AtmosExecOutput(){};

    void zero_outputs() { // Zeros everything except atmos_new_time
      temperature = pressure = density = speed_of_sound = dynamic_viscosity =
          mean_free_path = wind_angle_blowing_from = wind_angle_blowing_to =
          wind_vmag = 0.0;
      for (unsigned int i = 0; i < 3; i++) {
        wind_velocity_tc[i] = wind_velocity_td[i] = wind_velocity_eci[i] = 0.0;
      }
    }

  private:
    AtmosExecOutput (const AtmosExecOutput&);
    AtmosExecOutput & operator = (const AtmosExecOutput&);
};

#endif
