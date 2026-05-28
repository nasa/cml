/*******************************************************************************
PURPOSE:
  (Define the class MassBodyCompositeDispersions.
   This class manages the application of dispersions to composite-properties
   of a compound body.)

LIBRARY DEPENDENCIES:
  (../src/mass_body_composite_dispersions.cc)

ASSUMPTIONS:
  ((The mass properties have already been initialized before the update
    method gets called. Mass properties of the target body and all sub-bodies
    of the target body have been initialized to sensible values.)
    (JEOD mass-bodies are used within a JEOD mass-tree.))

LIMITATIONS:
  ((This model will not initialize mass properties, it will only modify them.))

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2018) (Antares) (Initial version))
   ((Gary Turner) (OSR) (Sept 2018) (Antares)
          (Split out the composite-to-core functionality to support other
           models with similar use-cases))
   ((Brent Caughron) (OSR) (Mar 2021) (Antares) (Secondary IV&V)))
*******************************************************************************/

#ifndef CML_MASS_BODY_COMPOSITE_DISPERSIONS_HH
#define CML_MASS_BODY_COMPOSITE_DISPERSIONS_HH

// JEOD headers:
#include "jeod/models/dynamics/mass/include/mass.hh" // jeod::MassBody
#include "jeod/models/dynamics/mass/include/mass_properties.hh" // jeod::MassProperties
#include "jeod/models/dynamics/body_action/include/body_action.hh" // jeod::BodyAction

#include "cml/models/utilities/cml_message/include/cml_message.hh"

// Local headers:
#include "mass_properties_dispersions.hh" // MassPropertiesDispersions

// Forward declaration
namespace jeod {
class DynManager;
}

/*******************************************************************************
MassBodyCompositeDispersions
PURPOSE:(Manages dispersions to be applied to the composite-properties.
         These must be applied after all of the body's mass-properties
         have been initialized; it *modifies* those properties, it does not
         initialize them.)
*******************************************************************************/
class MassBodyCompositeDispersions : public MassPropertiesDispersionsFlags,
                                     public jeod::BodyAction
{
// external references:
protected:
  jeod::MassBody & target_body; /* (--)
    The body whose composite-properties are being considered when assigning
    "nominal" and "disperse" properties. */

public:
  bool fail_if_mass_invalid;         /* (--)
    Flag determining whether non-physical mass properties (such as
    negative mass) in the adjustable-body should result in sim-termination.
    Default: false (allow non-physical properties). */
  jeod::MassProperties            nominal; /* (--)
    Set of nominal mass-properties. */
  MassPropertiesDispersions disperse;/* (--)
    Dispersion parameters, includes dispersion magnitudes if dispersions
    are computed internally, or dispersion values if computed externally,
    and controls to support options such as equal-distributions on
    different axes.*/
  jeod::MassBody * adjustable_body; /* (--)
    The body whose core-properties are being modified to meet the
    desired composite-properties of the target-body. This may be the
    target-body or a body subordinate to the target body in the mass-tree
    hierarchy.
    Note -- this is a pointer because it may be modified by the argument to
    the update(...) method. */
  jeod::MassBody * detach_sub_tree; /* (--)
    When evaluating the composite-properties, it may be desirable to
    consider only the composite-properties of some subset of the total
    mass tree. If this pointer is set, the mass tree will be detached at
    this point, removing this body and everything below it before the
    composite properties are considered. If this pointer is left null,
    the entire tree (below the target body) will be considered.
    Note -- this body must not be superior to the adjustable-body, or when
    it is detached it will take the adjustable-body with it.*/


  explicit MassBodyCompositeDispersions (jeod::MassBody & target_body_in);
  virtual ~MassBodyCompositeDispersions() {};

  // The apply method is used when the class is functioning as a
  // jeod::BodyAction. The jeod::DynManager argument is not needed, except as a
  // pass-through to jeod::BodyAction::apply(...).
  void apply(jeod::DynManager & dyn_manager);

  // Note - the update method is overloaded; these methods are used when
  // the class is being used independently of the jeod::BodyAction -- and
  // therefore of the Dynamics Manager.
  // - For the first option (no argument), the class member adjustable_body
  //   must have been set.
  // - For the second option (with argument), the body passed in becomes the
  //   adjustable_body; as with the class member, this can be the same as the
  //   target-body or lower in the mass-tree hierarchy. It cannot be above the
  //   target body because the core-properties of higher bodies do not affect
  //   the composite-properties of lower bodies.
  void update();
  void update( jeod::MassBody & adjustable);

private:
  void update_internal();
  void comp_to_core_updates();
  void process_dispersions();

  MassBodyCompositeDispersions(const MassBodyCompositeDispersions&);
  MassBodyCompositeDispersions & operator = (
                               const MassBodyCompositeDispersions&);
};
#endif
