/******************************** TRICK HEADER **********************************
PURPOSE:
   (Interface for the STD_76 atmosphere.
    Comprises atmosphere only; there is no associated winds model.)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (May 2016) (ANTARES)
       (New architecture to isolate the atmospheres from the master executive))
    ((Bingquan Wang) (OSR) (May 2017) (ANTARES)
       (Tweaked the code due to the refactoring of std_atmos_1976))
   )
********************************************************************************/

#include "../include/atmosphere_exec_std.hh"


/*****************************************************************************
Purpose:(Construct the class)
*****************************************************************************/
AtmosphereExec_STD::AtmosphereExec_STD(
       STD1976 & std_76_in,
       const double    & topod_alt,
       AtmosExecOutput & master_output)
  :
  AtmosphereExec_AtmosBase(master_output),
  std76(std_76_in),
  topodetic_altitude(topod_alt),
  hmsl(0.0)
{
  name = "STD1976";
}

/*****************************************************************************
Purpose:(main update loop for atmos, interface to std update, collects outputs)
*****************************************************************************/
void
AtmosphereExec_STD::update_atmos()
{
  hmsl = topodetic_altitude;
  std76.update(hmsl);

  out.density               = std76.density;
  out.temperature           = std76.kinetic_temperature;
  out.pressure              = std76.pressure;
  out.speed_of_sound        = std76.speed_of_sound;
  out.dynamic_viscosity     = std76.dynamic_viscosity;
  out.mean_free_path        = std76.mean_free_path;  
}
