/*******************************************************************************
Purpose:
  (Grail GRGM900C Lunar Gravity Field, 
   900x900 fully normalized gravity coefficients retrieved from the NASA
   Planetary Data System (PDS) web page 
   File: gggrx_0900c_sha.tab
   http://pds-geosciences.wustl.edu/grail/grail-l-lgrs-5-rdr-v1/grail_1001/shadr/
 
References:
  (((Lemoine, F.G., et al.)
    (GRGM900C: A degree 900 lunar gravity model from GRAIL primary and 
     extended mission data)
    (Geophys. Res. Lett., 41, 3382–3389, doi:10.1002/2014GL060027)
    (2014)))

Assumptions and Limitations:
  ((1. Gravity coefficients are fully normalized)
   (2. Gravity coefficients are static; no temporal variation.)
  )

Library Dependency:
  ((../src/GRGM900C_lunar_gravity.cc))

Programmers:
  (((Gary Turner) (OSR) (Oct 2015) (Antares)  (initial version)))
*******************************************************************************/

#ifndef GRGM900C_LUNAR_GRAVITY
#define GRGM900C_LUNAR_GRAVITY

#include "jeod/models/environment/gravity/include/spherical_harmonics_gravity_source.hh"

class GRGM900C_LunarGravity
{
 public:
 enum GRGM900C_resolution {
   LOW_RES_40   = 40,
   MED_RES_150  = 150,
   HIGH_RES_400 = 400,
   MAX_RES_900  = 900};

  GRGM900C_LunarGravity( GRGM900C_resolution resolution,
                         jeod::SphericalHarmonicsGravitySource & grav_source);
  ~GRGM900C_LunarGravity(){};
 protected:
  jeod::SphericalHarmonicsGravitySource & grav_source;
  void populate_low_res(); // sets the lowest coefficients
  void populate_med_res(); // sets coefficients 41-150
  void populate_high_res();// sets coefficients 151-400
  void populate_max_res_a(); // sets coefficients 401-625
  void populate_max_res_b(); // sets coefficients 626-792
  void populate_max_res_c(); // sets coefficients 793-900

 private:
  // Not implemented.
  GRGM900C_LunarGravity ( const GRGM900C_LunarGravity &);
  GRGM900C_LunarGravity & operator= ( const GRGM900C_LunarGravity &);
};
#endif



