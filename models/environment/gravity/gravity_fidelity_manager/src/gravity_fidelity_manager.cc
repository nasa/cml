/*******************************TRICK HEADER******************************
PURPOSE: (Manages the desired fidelity of the gravity model for one
          particular vehicle and one particular planetary body.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2019) (Antares) (initial))
   ((Daniel Ghan) (OSR) (April 2020) (Antares) (Made the table internal)))
**********************************************************************/

#include "jeod/models/utils/math/include/vector3.hh"

#include "../include/gravity_fidelity_manager.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
GravityFidelityManager::GravityFidelityManager(
    jeod::SphericalHarmonicsGravityControls & grav_controls_,
    const jeod::RefFrame                    & source_frame_,
    const jeod::RefFrame                    & target_frame_)
  :
  grav_controls(grav_controls_),
  source_frame(source_frame_),
  target_frame(target_frame_),
  distance_data(),
  deg_ord_data(),
  table(),
  distance(0.0),
  degree_order(0.0)
{
  // There are no versions of this model that do not use the table lookup, so
  // go ahead and subscribe it now.
  table.subscribe();
}

/*****************************************************************************
initialize
Purpose:(initializes the model and its dependencies)
*****************************************************************************/
void
GravityFidelityManager::initialize()
{
  table.load_independent_data( distance,
                               distance_data,
                               TableIndependentVariable::Prev);
  table.load_dependent_data( degree_order,
                             deg_ord_data);
  table.initialize();
  SubscriptionBase::initialize();
}

/*****************************************************************************
update
Purpose:(Main execution)
*****************************************************************************/
void
GravityFidelityManager::update()
{
  if (!active) {
    return;
  }

  double position[3];
  target_frame.compute_position_from( source_frame, position);
  distance = jeod::Vector3::vmag(position);

  table.update();

  // The simple-table-lookup generic capability uses double as the default
  // data type.  Therefore, even though degree and order are unsigned int
  // types, they must be stored as double.  Add 0.5 to prevent rounding error
  // (cast rounds down).
  grav_controls.degree =
  grav_controls.order = static_cast<unsigned int> (degree_order+0.5);

  grav_controls.spherical = (grav_controls.degree == 0);
}
