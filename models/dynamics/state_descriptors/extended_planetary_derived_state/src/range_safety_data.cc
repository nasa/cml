/*******************************************************************************
PURPOSE:
( Provides data for the range-safety analysis component of the
  planet-state model.)

ASSUMPTIONS AND LIMITATIONS:
(Data only)

PROGRAMMERS:
(((Gary Turner) (OSR) (June 2014)
                   (New implementation of PlanetaryDerivedState for Antares)))
*******************************************************************************/

#include "../include/range_safety_data.hh"


/*************************************************************************
Method: RangeSafetyData
Purpose: Constructor
*************************************************************************/
RangeSafetyData::RangeSafetyData()
   :
   XVRT_Azi(0.0),
   YVRT_Azi(0.0),
   T_pfix_pad{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
   T_pfix_pad_SEU{{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}},
   slant_range(0.0),
   XPad{},
   VPad{},
   NPad{},
   XVP{},
   pad_azimuth(0.0),
   XVRT(0.0),
   YVRT(0.0),
   ZVRT(0.0)
{
}
