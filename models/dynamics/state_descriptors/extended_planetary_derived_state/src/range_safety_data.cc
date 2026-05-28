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

#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/range_safety_data.hh"


/*************************************************************************
Method: RangeSafetyData
Purpose: Constructor
*************************************************************************/
RangeSafetyData::RangeSafetyData()
   :
   XVRT_Azi(0.0),
   YVRT_Azi(0.0),
   slant_range(0.0),
   pad_azimuth(0.0),
   XVRT(0.0),
   YVRT(0.0),
   ZVRT(0.0)
{
   jeod::Vector3::initialize(XPad);
   jeod::Vector3::initialize(VPad);
   jeod::Vector3::initialize(NPad);
   jeod::Vector3::initialize(XVP);
   jeod::Matrix3x3::identity(T_pfix_pad);
   jeod::Matrix3x3::identity(T_pfix_pad_SEU);
}
