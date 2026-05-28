/*******************************************************************************
   PURPOSE:
   ( Provides data for the range-safety analysis component of the
     planet-state model. Use of input and calculation of output data
     happens in ExtendedPlanetaryDerivedState)

   ASSUMPTIONS AND LIMITATIONS:
   (Data only)

   LIBRARY DEPENDENCY:
    (../src/range_safety_data.cc)

   PROGRAMMERS:
   (((Gary Turner) (OSR) (June 2014)
                      (New implementation of PlanetaryDerivedState for Antares))
    ((Dan Jordan) (Jacobs/JETS) (July 2017)
                      (EM1 V&V cleanup))
   )
*******************************************************************************/

#ifndef ANTARES_RANGE_SAFETY_DATA_HH
#define ANTARES_RANGE_SAFETY_DATA_HH


/*
   PURPOSE:
   ( Provides data for the range-safety analysis component of the
     planet-state model. Use of input and calculation of output data
     happens in ExtendedPlanetaryDerivedState)
*/
class RangeSafetyData
{
  public:
    RangeSafetyData();
    virtual ~RangeSafetyData(){};

    // INPUT VALUES
    /*FIXME -- There should probably be some checks on these azimuths in that they should
     * fall within reasonable ranges.  For example XVRT > pad_azimuth > YVRT in ILLs
     * use case, but there's no guarantee in the code that they fall in this range or
     * do not exceed a reasonable range (i.e. XVRT points opposite direction of vehicle
     * launch trajectory).  At the time of writing this, no one is using this model
     * so I'll leave that enhancement for CML at a later time - Dan Jordan July 2017 */
    double XVRT_Azi;   /* (rad)   Azimuth (clockwise from North) of XVRT axis */
    double YVRT_Azi;   /* (rad)   Azimuth (clockwise from North) of YVRT axis */

    // WORKING VALUES
    double T_pfix_pad[3][3];   /* (--)
          Rotation matrix from PFIX frame to PAD frame.  PAD frame is left-handed
          with x-axis oriented along azimuth angle East of North, z-axis up
          (elliptical/geodetic up), y completes.
          Frame origin is at launch pad.*/

    double T_pfix_pad_SEU[3][3];   /* (--)
          Rotation matrix from PFIX frame to PAD-SEU frame.  PAD-SEU frame is
          co-located with PAD frame.
          It is right-handed, oriented South-East-Up (x-y-z).*/

    // Output
    double slant_range;    /* (m)   Range magnitude between pad and vehicle */
    double XPad[3];        /* (m)   Position in Pad frame */
    double VPad[3];        /* (m/s)  Velocity in Pad frame */
    double NPad[3];        /* (m/s2) Acceleration in Pad frame */
    double XVP[3];         /* (m)   Position in vertical plane frame (SEU) */
    double pad_azimuth;    /* (rad)   Azimuth of vehicle in SEU pad frame */

    /* These are not currently used by anyone.  Dan Jordan 7/2017 */
    double XVRT;           /* (m)   Component of vehicle position on the XVRT axis.
                              XVRT axis is horizontal relative to the pad and perpendicular
                              to the North Impact Limit Line (ILL).  */
    double YVRT;           /* (m)   Component of vehicle position on the YVRT axis.
                              YVRT axis is horizontal relative to the pad and perpendicular
                              to the South Impact Limit Line (ILL).  */
    double ZVRT;           /* (m)   Altitude in pad frame */
};
#endif
