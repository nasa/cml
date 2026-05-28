/******************************* TRICK HEADER **********************************
PURPOSE:
    (Parachute function prototypes)

REFERENCE:
    (())

ASSUMPTIONS AND LIMITATIONS:
    ((For good or for awesome?))

LIBRARY DEPENDENCY:
     (())

PROGRAMMERS:
    (((R.Borland)          (GCS)   (Mar 2015) (We needs this))
     ((G Turner) (OSR) (June 2019) (Added "no units" specification for Trick17))
    )
*******************************************************************************/

#ifndef PARACHUTE_MAX_ITEMS_HH_
#define PARACHUTE_MAX_ITEMS_HH_

class MAX_ITEMS {

   public:

     static const int MAX_MORTARS    = 5; /* (--) Max number of mortars modeled*/
     static const int MAX_CHUTES     = 5; /* (--) Max number of chutes modeled */
     static const int MAX_ATTACH_PTS = 5; /* (--) Max number of attach points  */
     static const int MAX_HLINES     = 5; /* (--) Max number of harness lines  */
};

#endif
