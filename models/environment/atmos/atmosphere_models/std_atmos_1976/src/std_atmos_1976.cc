/********************************** TRICK HEADER ********************************
PURPOSE:
   (U.S. Standard Atmosphere 1976 model.)

REFERENCES:
  (([1] Anonymous, U.S. Standard Atmosphere, 1976
    U.S. Government Printing Office, 003-017-00323-0,
    Washington, D.C., October 1976.)

   ([2] Crues, Edwin Z., A 1976 Standard Atmosphere Program)

   ([2] Crues, Edwin Z., ATMOS76 Fortran77 Subroutine))

ASSUMPTIONS AND LIMITATIONS:
   (([1] Pressure exponentially interpolated from table values
         above 86 km.))

PROGRAMMERS:
   (((Joshua Gregg) (NASA) (June 2005) (ARES) (Initial implementation for ARES)))
    ((Thomas Phung) (JETS/HX5) (Dec 2014) (ARES) (FIXME model altitude violations when setup with EM1 CMSep scenario ))
    ((Bingquan Wang) (OSR) (April 2017) (ANTARES)
           (Refactor U.S. standard atmosphere model std_atmo_1976, including:
                 - Resigned the model code with C++ and do the necessary cleanup
                 - Change the way to set default data to make them as static
                 - Replace the logarithm interpolation with a linear one for the ratio of molecular
                   weights calculation in the geometric height range from 80km to 86km
                 - Add the calculation of mean free path since it's provided in model document
                 - Use the quick/binary search instead of linear loop))
   )

*********************************************************************************/

#include <cmath>
#include <algorithm>
#include <iterator>

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/std_atmos_1976.hh"
#include "std_atmos_formula.h"


STD1976::STD1976()
{
  initialize();
}

STD1976::~STD1976()
{
}

//initialize the atmosphere parameters as the values at mean sea level
void STD1976::initialize()
{
  altitude = 0.0;
  kinetic_temperature = T0;
  molecular_temperature = T0;
  pressure = pressure_table[0];
  density = cal_density(pressure, kinetic_temperature, molecular_weight);
  speed_of_sound = cal_cs(molecular_temperature);
  dynamic_viscosity = 0.0;
  molecular_weight = M0;
  mean_free_path = cal_mfp(kinetic_temperature, pressure);
  layer_number = 0;
}

//calculate the atmosphere paramters at the given geometric height
void STD1976::update(const double &altitude_in  //the given geometric height
                    )
{
  static const size_t rmwx_num = sizeof(RMWX_table)/sizeof(RMWX_table[0]);
  static const size_t ext_alt_num = sizeof(ext_alt_table)/sizeof(ext_alt_table[0]);
  static const size_t alt_num = get_alt_tbl_len();

  altitude = altitude_in;
  if(altitude < min_valid_alt) { //lower than the minimum geometic height
    CMLMessage::warn(__FILE__, __LINE__,
       "Lower than the minimum geometric height\n",
       "altitude = ", altitude, " - Altitude is below ", min_valid_alt, " m (out of range low), and "
       "is trimmed.\n");
    altitude = min_valid_alt ;
  }
  else if(altitude > max_valid_alt) { //higher the maximum geomaric height
    CMLMessage::warn(__FILE__, __LINE__,
       "Higher than the maximum geometric height\n",
       "altitude = ", altitude, " - Altitude is above ", max_valid_alt, " m (out of range high), and "
       "is trimmed.\n");
    altitude = max_valid_alt ;
  }

  //to calcualte mean molecular weight (kg/kmol)
  double mw_ratio = 1.0;
  if(altitude <= RMWX_table[0]) {  //lower 80 km
    molecular_weight = M0;
  }
  else if(altitude <= RMWX_table[rmwx_num-1]) { //from 80 km through 86 km
    mw_ratio = interp(RMWX_table, RMWY_table, rmwx_num, altitude);
    molecular_weight = M0*mw_ratio;
  }
  else { //above 86 km
    molecular_weight = log_interp(ext_alt_table, MWY_table, ext_alt_num, altitude);
    mw_ratio = molecular_weight/M0;
  }

  //calculate geopotential height
  const double H = altitude<alt_table[7] ? cal_geop_h(altitude) : altitude;

  //to find out the layer number
  layer_number = 0;  //lower mean sea level is considered as layer 0
  if (altitude > alt_table[0]) { //form 0 km to 1000 km
    const double *itr = std::upper_bound(alt_table, alt_table+alt_num, H);
    if (itr == alt_table+alt_num) {
      layer_number = (int)(alt_num-1);
    }
    else {
      layer_number = (int)(std::distance(alt_table, itr)-1);
    }
  }

  //to calculate kinetic and molecular temperature (K)
  if (layer_number < 7) { //lower 86 km
    molecular_temperature = moltemp_table[layer_number] + gradient_table[layer_number]*(H-alt_table[layer_number]);
    kinetic_temperature = molecular_temperature*mw_ratio;
  }
  else { //above 86 km
    if (layer_number == 7) { //from 86 km to 91 km
      kinetic_temperature = moltemp_table[layer_number];
    }
    else if (layer_number == 8) { //from 91 km to 110 km
      const double t = (altitude-alt_table[layer_number])/19942.9;
      kinetic_temperature = 263.1905 - 76.3232*std::sqrt(1.0-t*t);
    }
    else if (layer_number == 9) { //from 110 km to 120 km
      kinetic_temperature = moltemp_table[layer_number] + gradient_table[layer_number]*(altitude-alt_table[layer_number]);
    }
    else { // above 120 km
      const double eta = (1.0 + alt_table[10]/r0) / (1.0 + altitude/r0) * (altitude-alt_table[10])/1000.0;
      kinetic_temperature = 1000.0 - (1000.0 - moltemp_table[10])/std::exp(0.01875*eta);
    }

    molecular_temperature = kinetic_temperature/mw_ratio;
  }

  //to calculae pressure (N/m2)
  if (layer_number < 7) { //lower 86 km
    if (layer_number==0 || layer_number==2 ||
        layer_number==3 || layer_number==5 || layer_number==6) { // for the cases of non-zero Lm,b
      const double c1 = g0*M0/Rs/gradient_table[layer_number];
      const double t_ratio = moltemp_table[layer_number]/molecular_temperature;
      pressure = pressure_table[layer_number]*std::pow(t_ratio, c1);
    }
    else {  // for the cases of zero Lm,b
      const double c1 = g0*M0/Rs/moltemp_table[layer_number]*(H-alt_table[layer_number]);
      pressure = pressure_table[layer_number]/std::exp(c1);
    }
  }
  else { //above 86 km
    pressure = log_interp(ext_alt_table, PY_table, ext_alt_num, altitude);
  }

  //to calculate mass density (kg/m3)
  density  = cal_density(pressure, kinetic_temperature, molecular_weight);

  //to calculate speed of sound (m/s)
  speed_of_sound = layer_number<7 ? cal_cs(molecular_temperature) : Cs_86;

  //to calculate dynamic viscosity (N*s/m2)
  dynamic_viscosity = kinetic_temperature*std::sqrt(kinetic_temperature)/(kinetic_temperature + Su)*beta;

  //to calculate mean free path (m)
  mean_free_path = cal_mfp(kinetic_temperature, pressure);
}
