/******************************** TRICK HEADER **********************************
PURPOSE:
  (To define the constants and coefficients used in the compution of U.S. Standard 
   Atmosphere 1976 model.)

REFERENCES:
  ((U.S. STANDARD ATMOSPHERE 1976, N77-16482,
    https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770009539.pdf))

PROGRAMMERS:
  (((Bingquan Wang) (OSR) (April 2017) (ANTARES) (Refactored  version from previous 
                    STD1976__std_atmos_1976__default_data.hh, including:
                      - Change the way to set default data to make them as static
                      - Correct the data table of RMWY_table
                      - Remove the table of PX_table[87], since it's duplicated with table MWX_table[87]
                      - Correct the data of table MWY_table at 86km to be consistent with data table RMWY_table))
  )

********************************************************************************/

#include <cmath>

#include "trick/trick_math.h"  //for M_PI
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/std_atmos_1976.hh"
#include "std_atmos_formula.h"


const double STD1976::min_valid_alt = -5000.0;
const double STD1976::max_valid_alt = 1000000.0;


const double STD1976::Su = 110.4;
const double STD1976::beta = 1.458E-6;
const double STD1976::Tm_86 = 186.9459;
const double STD1976::g0 = 9.80665;
const double STD1976::M0 = 28.9644;
const double STD1976::r0 = 6.356766E6;
const double STD1976::Rs = 8314.32;
const double STD1976::T0 = 288.15;
const double STD1976::Na = 6.022169E26;
const double STD1976::sigma = 3.65E-10;
const double STD1976::mfp_coeff = STD1976::Rs / (STD1976::Na * STD1976::sigma * STD1976::sigma * M_PI * std::sqrt(2)); 
const double STD1976::gamma = 1.4;
const double STD1976::Cs_86 = cal_cs(STD1976::Tm_86);


const double 
STD1976::alt_table[13]      = {      0.,  11000.,  20000.,  32000.,  47000.,  51000.,  71000.,  86000.,  91000.,   110000.,   120000.,   500000.,  1000000.};
const double 
STD1976::moltemp_table[13]  = { 288.150, 216.650, 216.650, 228.650, 270.650, 270.650, 214.650, 186.867, 186.867,   240.000,   360.000,   999.236,  1000.000};
const double 
STD1976::gradient_table[13] = { -0.0065,     0.0,   0.001,  0.0028,     0.0, -0.0028,  -0.002,     0.0,     0.0,     0.012,       0.0,       0.0,       0.0};
const double 
STD1976::pressure_table[13] = {101325.0, 22632.0,  5474.8,  868.01,  110.90,  66.938,  3.9564, 0.37338, 0.15381, 0.0071042, 0.0025382, 3.0236E-7, 7.5138E-9};


const double 
STD1976::RMWX_table[24] = { 80000.0,  80500.0,  80506.9,  81000.0,  81019.6,  81500.0,  81532.5,  82000.0,  82045.4,  82500.0,  82558.6, 83000.0,
                            83071.5,  83500.0,  83584.8,  84000.0,  84098.0,  84500.0,  84611.4,  85000.0,  85124.8,  85500.0,  85638.4, 86000.0};

//FIXME: the ratio of M/M0 for altitude 85638.4 m in Table 8 of model document N77-16482 is 0.999679, which seems not right since it causes an obvious spiky disconnuity. 
//Below I changed it as 0.999624 by refering the calcualtion at http://www.aerospaceweb.org/design/scripts/atmosphere to get rid of this discontuity. Plus document
//N77-16482 also mentions that the purpose of this M/M0 ratio table is "... to satisfy the boundary conditions of M=M0=28.9644 at 80km, and M=28.9522 at 86km, and to 
//satisfy a condition of smoothly decreasing first differences in M within the height interval 80 to 86km."
const double
STD1976::RMWY_table[24] = {1.000000, 0.999996, 0.999996, 0.999989, 0.999988, 0.999971, 0.999969, 0.999941, 0.999938, 0.999909, 0.999904, 0.999870,
                           0.999864, 0.999829, 0.999822, 0.999786, 0.999778, 0.999741, 0.999731, 0.999694, 0.999681, 0.999641, 0.999624, 0.999579};


const double 
STD1976::ext_alt_table[87] = {  86000.0,   87000.0,   88000.0,   89000.0,   90000.0,   91000.0,   93000.0,   95000.0,   97000.0,   99000.0,  101000.0,  103000.0,  105000.0,
                               107000.0,  109000.0,  110000.0,  111000.0,  112000.0,  113000.0,  114000.0,  115000.0,  116000.0,  117000.0,  118000.0,  119000.0,  120000.0,
                               125000.0,  130000.0,  135000.0,  140000.0,  145000.0,  150000.0,  160000.0,  170000.0,  180000.0,  190000.0,  200000.0,  210000.0,  220000.0,
                               230000.0,  240000.0,  250000.0,  260000.0,  270000.0,  280000.0,  290000.0,  300000.0,  310000.0,  320000.0,  330000.0,  340000.0,  350000.0,
                               360000.0,  370000.0,  380000.0,  390000.0,  400000.0,  410000.0,  420000.0,  430000.0,  440000.0,  450000.0,  460000.0,  470000.0,  480000.0,
                               490000.0,  500000.0,  525000.0,  550000.0,  575000.0,  600000.0,  625000.0,  650000.0,  675000.0,  700000.0,  725000.0,  750000.0,  775000.0,
                               800000.0,  825000.0,  850000.0,  875000.0,  900000.0,  925000.0,  950000.0,  975000.0, 1000000.0};
const double 
STD1976::MWY_table[87]     =  { 28.9522,     28.95,     28.94,     28.93,     28.91,     28.89,     28.82,     28.73,     28.62,     28.48,     28.30,     28.10,     27.88,
                                  27.64,     27.39,     27.27,     27.14,     27.02,     26.90,     26.79,     26.68,     26.58,     26.48,     26.38,     26.29,     26.20,
                                  25.80,     25.44,     25.09,     24.75,     24.42,     24.10,     23.49,     22.90,     22.34,     21.81,     21.30,     20.83,     20.37,
                                  19.95,     19.56,     19.19,     18.85,     18.53,     18.24,     17.97,     17.73,     17.50,     17.29,     17.09,     16.91,     16.74,
                                  16.57,     16.42,     16.27,     16.13,     15.98,     15.84,     15.70,     15.55,     15.40,     15.25,     15.08,     14.91,     14.73,
                                  14.54,     14.33,     13.76,     13.09,     12.34,     11.51,     10.62,      9.72,      8.83,      8.00,      7.24,      6.58,      6.01,
                                   5.54,      5.16,      4.85,      4.60,      4.40,      4.25,      4.12,      4.02,      3.94};
const double 
STD1976::PY_table[87]      = {3.7338E-1, 3.1259E-1, 2.6173E-1, 2.1919E-1, 1.8359E-1, 1.5381E-1, 1.0801E-1, 7.5966E-2, 5.3571E-2, 3.7948E-2, 2.7192E-2, 1.9742E-2, 1.4477E-2,
                              1.0751E-2, 8.1142E-3, 7.1042E-3, 6.2614E-3, 5.5547E-3, 4.9570E-3, 4.4473E-3, 4.0096E-3, 3.6312E-3, 3.3022E-3, 3.0144E-3, 2.7615E-3, 2.5382E-3,
                              1.7354E-3, 1.2505E-3, 9.3568E-4, 7.2028E-4, 5.6691E-4, 4.5422E-4, 3.0395E-4, 2.1210E-4, 1.5271E-4, 1.1266E-4, 8.4736E-5, 6.4756E-5, 5.0149E-5,
                              3.9276E-5, 3.1059E-5, 2.4767E-5, 1.9894E-5, 1.6083E-5, 1.3076E-5, 1.0683E-5, 8.7704E-6, 7.2285E-6, 5.9796E-6, 4.9630E-6, 4.1320E-6, 3.4498E-6,
                              2.8878E-6, 2.4234E-6, 2.0384E-6, 1.7184E-6, 1.4518E-6, 1.2291E-6, 1.0427E-6, 8.8645E-7, 7.5517E-7, 6.4468E-7, 5.5155E-7, 4.7292E-7, 4.0642E-7,
                              3.5011E-7, 3.0236E-7, 2.1200E-7, 1.5137E-7, 1.1028E-7, 8.2130E-8, 6.2601E-8, 4.8865E-8, 3.9048E-8, 3.1908E-8, 2.6611E-8, 2.2599E-8, 1.9493E-8,
                              1.7036E-8, 1.5051E-8, 1.3415E-8, 1.2043E-8, 1.0873E-8, 9.8635E-9, 8.9816E-9, 8.2043E-9, 7.5138E-9};


size_t STD1976::get_alt_tbl_len()
{
  const size_t alt_tbl_len = sizeof(alt_table)/sizeof(double);
  for (size_t i=1; i<alt_tbl_len; ++i) {
    if (alt_table[i] <= alt_table[i-1]) {
      CMLMessage::fail(__FILE__, __LINE__, "Data out of order.\n", "Data in table alt_table should be in increasing order.");
    }
  }

  const size_t ext_alt_tbl_len = sizeof(ext_alt_table)/sizeof(double);
  for (size_t i=1; i<ext_alt_tbl_len; ++i) {
    if (ext_alt_table[i] <= ext_alt_table[i-1]) {
      CMLMessage::fail(__FILE__, __LINE__, "Data out of order.\n", "Data in table ext_alt_table should be in increasing order.");
    }
  } 

  return alt_tbl_len;
}
