/*******************************************************************************

Purpose:
  (Provides a predictive capability to SphericalHarmnicsGravityControls without needing to
  recompute the full spherical harmonics)

Library dependencies:
  ((../src/spherical_harmonics_fast_gravity_controls.cc))

Programmers:
 (((Gary Turner) (OSR) (May 2014) (Antares) (Implementation of Blair Thompson's
                               algorithm originally written for earlier JEOD))
  ((Bingquan Wang) (OSR) (June 2017) (Antares) (Code cleanup for IV&V))
 )

*******************************************************************************/

#ifndef JEOD_SPHERICAL_HARMONICS_FAST_GRAVITY_CONTROLS_HH
#define JEOD_SPHERICAL_HARMONICS_FAST_GRAVITY_CONTROLS_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/environment/gravity/include/spherical_harmonics_gravity_controls.hh"

class SphericalHarmonicsFastGravityControls : public jeod::SphericalHarmonicsGravityControls {

// Member data

 public:
   bool fast_enabled; /* (--) Model level flag. Use regular SphHarmGravCon if not. */

   unsigned int count_limit; /* (--)
        The maximum number of fast gravity computations before regular
        computation is used again.*/

   bool compute_count_limit; /* (--)
        Base the value of count_limit automatically on the threshold value
        (below).  Otherwise, it is a user-settable value.*/

   double threshold_ratio_delta_acc; /* (--)
        The target ratio between the magnitudes of the (difference between
        the fast and regular grav accel vectors) and the (regular grav
        accel vector).  Used only if compute_count_limit. */

 private:
   bool available; /* (--)
        Internal flag, set off if tolerance exceeded
        and on with each full computation.*/

   bool first_pass; /* (--)
        Internal flag, set for the first pass through the algorithm.
        Prevents the count_limit getting reset because the comparison will
        be far out of tolerance on the first pass.*/

   unsigned int count; /* (--) count since last full computation */

   double reference_pos_pfix[3]; /* (m)  position at last computation */

   double delta_pos[3]; /* (m) position change since last computation */

   double reference_accel_pfix[3]; /* (m/s2)  acceleration at last computation */

   double reference_gradient_pfix[3][3]; /* (1/s2)
       gravity gradient at last computation expressed in pfix reference frame.*/

   double reference_gradient_inrtl[3][3]; /* (1/s2)
       gravity gradient at last computation expressed in inrtl ref frame.*/

   double reference_potential; /* (J/kg) grav potential at last computation */


//Methods

 public:
   SphericalHarmonicsFastGravityControls();

   virtual ~SphericalHarmonicsFastGravityControls(){};

   virtual void initialize_control ( jeod::GravityManager &grav_manager);

   virtual void  calc_nonspherical ( const double integ_pos[3], // In: unused
                                     const double inertial_position[3], // In: M Point of interest, inrtl frm
                                     const jeod::GravityIntegFrame & grav_source_frame, // In: Unused
                                     double body_grav_accel[3], // Out: M/s2 Accel for given grav body
                                     double dgdx[3][3],         // Out: 1/s2 Gradient for given grav body
                                     double & pot);             // Out: --   Potential

   void reset_count_limit() { if (compute_count_limit) count_limit = 1;};

 // Make the copy constructor and assignment operator private
 // (and unimplemented) to avoid erroneous copies
 private:

   /**
    * Not implemented.
    */
   SphericalHarmonicsFastGravityControls (
      const SphericalHarmonicsFastGravityControls &);

   /**
    * Not implemented.
    */
   SphericalHarmonicsFastGravityControls & operator= (
      const SphericalHarmonicsFastGravityControls &);


};

#endif
