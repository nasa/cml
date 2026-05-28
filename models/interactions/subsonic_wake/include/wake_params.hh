/*******************************TRICK HEADER******************************
PURPOSE: (Definition of the bodies in the wake profile.)

LIBRARY DEPENDENCY:
   ((../src/wake_bodies.cc))

PROGRAMMERS:
  ( ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
    ((Bingquan Wang) (OSR) (Oct 2016) (Antares)
                (Library dependency warning fixing)))
**********************************************************************/
#ifndef WAKE_PARAMS_HH
#define WAKE_PARAMS_HH

/*****************************************************************************
WakeParamsSub
Purpose:(A set of distance thresholds for the rev-flow and PRF models)
*****************************************************************************/
class WakeParamsSub
{
 public:
  double  start_dist; /* (m)
       Separation distance between bodies where sub-model starts */
  double  stop_dist;  /* (m)
       Separation distance between bodies where sub-model stops  */
  double  radial_dist;  /* (m)
       Radial separation distance between bodies where sub-model stops */

  WakeParamsSub()
    :
    start_dist(0.0),
    stop_dist(0.0),
    radial_dist(0.0)
  {};
};

/*****************************************************************************
WakeParams
Purpose:(Replaces the struct PARAM.
         Provides the controlling parameters for the model.)
*****************************************************************************/
class WakeParams
{
 public:
  bool generate_distance_override; /* (--)
       If there is no force, no reverse flow and no PRF to compute, the model
       will immediately return by default, with no computations.  This flag
       forces the computation of the separation, trailing and radial distances
       even when they are not going to be used for the conventional outputs.*/
  double grid_origin[3]; /* (m)
       Vector from bodyA struc origin to grid_origin. In bodyA struc frame.*/
  double  mach_off; /* (--)
       Mach number below which to switch off the wake model */
  double main_body_diameter; /* (m)
       Reference diameter of the wake-generating body.*/
  WakeParamsSub rev_flow; /* (--) params for rev-flow model */
  WakeParamsSub prf;      /* (--) params for prf model */

  WakeParams()
    :
    generate_distance_override(false),
    grid_origin(),
    mach_off(0.0),
    main_body_diameter(0.0),
    rev_flow(),
    prf()
  {};
};
#endif
