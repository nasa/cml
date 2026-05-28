/*************************************************************
PURPOSE:
   (Provide default data for launch and landing sites.

PROGRAMMERS:
    ((Gary Turner) (Odyssey) (January 2015) (Antares) (Initial version)))

*************************************************************/

#define _USE_MATH_DEFINES /// M_PI_2
#include <cmath> // M_PI_2
#include "trick/constant.h" // DTR, degrees to radians.

#include "../../include/pfix_reference_point.hh" // enumeration
#include "../include/range_default_data.hh"



/*****************************************************************************
launch_pad_KSC_39B
Purpose:(Sets the launch pad to KSC 39B)
Reference:(Default KSC Pad Due East Range Reference.
           Launch Pad 39B Inputs - NSTS 08209 Vol 1 RevB Table 7.1)
*****************************************************************************/
void
RangeDefaultData::launch_pad_KSC_39B(
       RangeFromPfixReference & pad)
{
  pad.reference_data.position_type = PfixReferencePoint::topodetic;
  pad.reference_data.longitude = -80.6207970 * DTR;
  pad.reference_data.latitude = 28.627213 * DTR;
  pad.reference_data.altitude = 0.0;

  pad.reference_data.direction_type = PfixReferencePoint::topodetic;
  pad.reference_data.azimuth = M_PI_2;
}
