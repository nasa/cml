/******************************** TRICK HEADER ********************************** 
PURPOSE:
   (To compute the atmosphere parameters according to U.S. Standard Atmosphere 1976 model.)

REFERENCES:
   ((U.S. STANDARD ATMOSPHERE 1976, N77-16482,
     https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770009539.pdf))

ASSUMPTIONS AND LIMITATIONS:
   ((Above the geometric height of 86km, use the data tables of pressure and mean 
     molecular weight to interpolate to avoid the atmosphere number density integration.))

LIBRARY DEPENDENCY:
   ((../src/std_atmos_1976.cc)
    (../src/std_atmos_1976_dd.cc)
   )

PROGRAMMERS:
   (((Joshua Gregg) (NASA) (June 2005) (ARES) (Initial implementation for ARES))
    ((Bingquan Wang) (OSR) (April 2017) (ANTARES) 
                     (Refactoring and cleaning up, including:
                       - Resigned the data structure  with C++ 
                       - Change the way to set default data to make them as static
                       - Remove the parameter of alt_offset for altitude offset between 
                         mean sea level and Fischer ellipsoid
                       - Add the calculation of mean free path since it's provided in model document
                       - Remove the table of PX_table[87], since it's duplicated with table MWX_table[87]))
   )

********************************************************************************/

#ifndef STD_ATMOS_1976_H
#define STD_ATMOS_1976_H

#include <cstddef>
/** The main class to implement the computation of standard atmosphere model 1976 */
class STD1976 
{
public:
   //below using const references for the outputs and also keep compatible with Trick
   double altitude;                       /* (m)           Geometric input altitude */
   double kinetic_temperature ;           /* (K)           Kinetic temperature       */
   double molecular_temperature  ;        /* (K)           Molecular temperature     */
   double pressure ;                      /* (N/m2)        Pressure                  */
   double density ;                       /* (kg/m3)       Density                   */
   double speed_of_sound ;                /* (m/s)         Speed of sound            */
   double dynamic_viscosity ;             /* (N*s/m2)      Dynamic viscosity         */
   double molecular_weight ;              /* --            Molecular weight          */
   double mean_free_path;                 /* (m)           Mean free path            */
   int    layer_number ;                  /* --            Layer number               */
  
   /** constructor, and initialize the 
   *   state as that at mean sea level.
   */
   STD1976();

   /** destructor */
   ~STD1976();

   /** compute the atmosphere parameters above at the given geometric height */
   void update(
                const double &altitude_in /* (m) The given geometric height */
              );

public:
   //use the static variables to define the constants and coefficients since they are class-wise.
   //This will put these data into data segment to avoid using stack.
   static const double min_valid_alt;           /* (m)              Lowest valid altitude for std1976 model */
   static const double max_valid_alt;           /* (m)              Highest valid altitude for std1976 model */

   static const double Su;                      /* (K)              Sutherland's Constant */
   static const double beta;                    /* --               Constant for calculating dynamic viscosity */
   static const double Tm_86;                   /* (K)              Molecular temperature at 86 km */
   static const double g0;                      /* (m/s2)           Sea level value of acceleration of gravity */
   static const double M0;                      /* --               Sea level molecular weight */
   static const double r0;                      /* (m)              Effective Earth radius */
   static const double Rs;                      /* --               the universal gas constant */
   static const double T0;                      /* (K)              Sea level temperature */
   static const double Na;                      /* --               the Avogadro constant */
   static const double sigma;                   /* (m)              the effective mean collision diameter */
   static const double mfp_coeff;               /* (N/K/m)          the coefficient to calculate mean free path */
   static const double gamma;                   /* --               A constant taken to represent the ratio of specific
                                                                     heat at constant pressure to the specific heat at
                                                                     constant volume, and used in defining Cs */
   static const double Cs_86;                   /* (m/s)            Speed of sound at 86 km */
   

   static const double alt_table[13];           /* (m)          Layer boundary altitudes (geometric) */
   static const double moltemp_table[13];       /* (K)          Molecular temperature at layer boundaries */
   static const double gradient_table[13];      /* (K/km)       Temperature gradient table */ 
   static const double pressure_table[13];      /* (N/m2)       Pressure table */

   static const double RMWX_table[24];          /* (m)          Altitudes for molecular weight ratio table */
   static const double RMWY_table[24];          /* --           Molecular weight ratio table */

   static const double ext_alt_table[87];       /* (m)          Altitudes table above 86 km (geometric) */
   static const double MWY_table[87];           /* --           Molecular weight table above 86 km */
   static const double PY_table[87];            /* (N/m2)       Pressure table  above 86 km */


   //to verify the height data tables in order
   static size_t get_alt_tbl_len();

private:
   //initialize the output atmosphere parameters as the values at mean sea level
   void initialize();
} ;

#endif // STD_ATMOS_1976_H

