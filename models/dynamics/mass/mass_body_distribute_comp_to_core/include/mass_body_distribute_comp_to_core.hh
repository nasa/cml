/*******************************************************************************
Purpose:
  (Define the class MassBodyDistributeCompToCore
   This class sets the core-mass-properties of one body to meet specified
   composite-mass-properties of another body further up the tree.
  )

Library dependencies:
  ((../src/mass_body_distribute_comp_to_core.cc))

Assumptions:
  ((The mass properties have already been initialized before the update
    method gets called.)
    (JEOD mass-bodies are used within a JEOD mass-tree.))

Limitations:
  ((This model will not initialize mass properties, it will only modify them))

Programmers:
  (((Gary Turner) (OSR) (Sept 2018) (Antares) (Initial version))
  )
*******************************************************************************/

#ifndef CML_MASS_BODY_DISTRIBUTE_COMP_TO_CORE_HH
#define CML_MASS_BODY_DISTRIBUTE_COMP_TO_CORE_HH

#include "jeod/models/dynamics/mass/include/mass.hh" // jeod::MassBody
#include "jeod/models/dynamics/mass/include/mass_properties.hh" // jeod::MassProperties
#include "jeod/models/dynamics/mass/include/mass_point_state.hh" // MassPointState

#include "cml/models/utilities/cml_message/include/cml_message.hh"


/*****************************************************************************
MassBodyDistributeCompToCore
Purpose:(Allows the setting of composite-mass properties and internal modifies
         core properties of a MassBody down the mass-tree (this, child,
         child-of-child, etc.) such that the desired composite-properties
         are satisfied.
*****************************************************************************/
class MassBodyDistributeCompToCore
{
 protected: // External references
  jeod::MassBody & target_body; /* (--) The body whose composite-properties are
                                  specified. */
  jeod::MassBody & adjustable_body; /* (--) The body whose core-properties are being
                                      adjusted in order to meet the specified
                                      composite-properties of the target-body.*/
 public:
  bool fail_if_mass_invalid; /* (--) Determines the model's behaviour if the new
                        mass properties are invalid.
                        If true:  simulation is terminated
                        If false: simulation continues with an error message
                        Default:  true */
 protected:
  jeod::MassPointState adjustable_struc_wrt_target_struc; /* (--)
                        position and orientation of the adjustable-body
                        structure-point wrt the target-body structure-point. */
  jeod::MassProperties target_properties; /* (--) target properties generated from
                        a combination of nominal and dispersed properties. */
  double old_mass; /* (kg) the original mass of the adjustable_body. */

 public:
  MassBodyDistributeCompToCore( jeod::MassBody & adjustable_body,
                                jeod::MassBody & target_body);
  virtual ~MassBodyDistributeCompToCore() {};


  bool update( const jeod::MassProperties & new_target_props);

 protected:
  bool check_configuration();
  void generate_new_position();
  void generate_new_inertia();
  void check_for_valid_mass_properties();

  MassBodyDistributeCompToCore(const MassBodyDistributeCompToCore&);
  MassBodyDistributeCompToCore & operator = (
                               const MassBodyDistributeCompToCore&);
};
#endif
