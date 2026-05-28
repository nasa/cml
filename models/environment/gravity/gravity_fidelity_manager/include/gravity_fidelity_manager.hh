/*******************************TRICK HEADER******************************
PURPOSE: (Manages the desired fidelity of the gravity model for one
          particular vehicle and one particular planetary body.)

LIBRARY DEPENDENCY:
   ((../src/gravity_fidelity_manager.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2019) (Antares) (initial))
   ((Daniel Ghan) (OSR) (April 2020) (Antares) (Made the table internal)))
**********************************************************************/
#ifndef CML_GRAVITY_FIDELITY_MANAGER_HH
#define CML_GRAVITY_FIDELITY_MANAGER_HH

#include "jeod/models/environment/gravity/include/spherical_harmonics_gravity_controls.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame.hh"
#include "cml/models/utilities/table_interp_cpp/include/simple_table_lookup.hh"
#include <vector>

class GravityFidelityManager : public SubscriptionBase
{
 protected: // External references
  jeod::SphericalHarmonicsGravityControls & grav_controls; /* (--)
       Reference to the JEOD gravity controls being managed by this event.*/
  const jeod::RefFrame & source_frame; /* (--)
       One of the two frames used for determining the controlling distance.
       Typically, this would be the planet reference frame*/
  const jeod::RefFrame & target_frame; /* (--)
       The other of the two frames used for determining the controlling
       distance.  Typically, this would be the vehicle reference frame*/

 public:
  std::vector<double> distance_data; /* (m) Cut-off radius for each level of
       gravity fidelity. */
  std::vector<double> deg_ord_data; /* (--) Degree and order to which the
       gravity model will be set if the distance between the two frames
       is greater than the corresponding cut-off radius.
       For example, if the distance between source_frame and target_frame
       is between the second and third elements of radii, the degree and
       order of grav_controls will be set to the second element of deg_order. */

 protected:
  SimpleTableLookup table; /* (--)
      The table containing the desired degree/order values and the
      corresponding distances at which they should be implemented.
      NOTE - table behavior defaults to using proximity-overrides if the
             independent variable is with 1E-9 of the bin-width of
             one of the calibrated values.*/
  double distance; /* (m) The distance between the two frames. */
  double degree_order; /* (--)
      The desired degree and order of the gravity field.
      This is a "double" type because that is the native type of the
      SimpleTableLookup; the value will be cast to an unsigned int for
      assignment to the gravity model after the table-lookup has populated it.*/

 public:
  GravityFidelityManager(  jeod::SphericalHarmonicsGravityControls & grav_controls,
                           const jeod::RefFrame                    & source_frame,
                           const jeod::RefFrame                    & target_frame);
  virtual ~GravityFidelityManager(){};

  void initialize();
  void update();

 private:
  // Unimplemented copy constructor and assignment operator
  GravityFidelityManager (const GravityFidelityManager&);
  GravityFidelityManager & operator = (const GravityFidelityManager&);
};
#endif
