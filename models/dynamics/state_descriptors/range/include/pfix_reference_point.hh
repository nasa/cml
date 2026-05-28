/********************************* TRICK HEADER *******************************
Purpose:
   (Defines the planet-fixed reference-point for computations of range.)

Library Dependency:
   ((../src/pfix_reference_point.cc))

Programmers:
   (((Gary Turner)    (OSR)        (Sept 2014)  (Initial version))
    ((Debbie Merritt) (NASA)       (Nov 2014)   (Added units definition to reference parameters))
    ((Dan Jordan)     (Jacobs/JETS) (June 2017) (EM1 V&V cleanup)))
*******************************************************************************/

#ifndef CML_PFIX_REFERENCE_POINT
#define CML_PFIX_REFERENCE_POINT

#include "jeod/models/utils/planet_fixed/planet_fixed_posn/include/planet_fixed_posn.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

class PfixReferencePoint : public jeod::PlanetFixedPosition
{
 public:
  enum PositionType {
    undefined,
    topodetic,
    topocentric,
    inertial,
    pfix
  };

  // INPUT
  PositionType position_type;   /* (--)  enumeration for specifying position*/
  PositionType direction_type;  /* (--)  enumeration for specifying direction*/

  double latitude;     /* (rad)
     Latitude of ref point. Distinction between topocentric and topodetic
     is provided by selection of "position_type". */
  double longitude;    /* (rad)
     Longitude of ref point. Distinction between topocentric and topodetic
     is provided by selection of "position_type". */
  double altitude;     /* (m)
     Altitude of ref point. Distinction between topocentric and topodetic
     is provided by selection of "position_type". */


  double position[3];  /* (m)
     Cartesian position vector. Distinction between inertial and
     planet-fixed is provided by selection of "position_type". */

  double azimuth;      /* (rad)
       Azimuth for specifying direction, measured from North towards East.
       Use of azimuth requires setting direction_type to be either
       topocentric or topodetic, but topocentric and topodetic azimuths
       are equivalent so either is sufficient.*/
  double direction[3]; /* (--)
       Cartesian vector specifying direction, not necessarily a unit vector.
       Use of this vector to specify direction requires setting
       direction_type to be either inertial or pfix.  Selection of which
       identifies how the values in the vector should be interpreted. */

  PfixReferencePoint();
  virtual ~PfixReferencePoint(){};

  void set_point (double & ref_radius, bool & ref_point_set);
  void set_direction (bool & use_polar, bool & ref_point_set);

 private:
   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  PfixReferencePoint(const PfixReferencePoint& rhs);
  PfixReferencePoint & operator = (const PfixReferencePoint&);
};

#endif
