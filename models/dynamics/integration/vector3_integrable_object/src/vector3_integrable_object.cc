/************************** TRICK HEADER***************************************
PURPOSE:
(Define the methods for adding a 3-vector to a jeod::DynBody and integrating it alongside the jeod::DynBody)

LIMITATIONS:
(Requires JEOD v 3.2, or modifications to earlier versions of JEOD to provide
 the jeod::DynBody with additional capabilities not available in 3.1 and earlier:
 1.  With the instruction to add an integrable object, a check must be made
     that the object has not already been added
 2.  The jeod::DynBody must have the ability to remove an integrable object from
     its list of associated integrable objects.)

REFERENCE:
(((JEOD Thermal-rider model thermal_integrable_object)))

PROGRAMMERS:
(((Gary Turner) (OSR) (October, 2015) (initial version))
 ((Brent Caughron) (OSR) (Sept. 2017) (IV&V code review)))
*******************************************************************************/

#include <cstring> // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/dynamics/dyn_manager/include/dynamics_integration_group.hh"

#include "../include/vector3_integrable_object.hh"


/*****************************************************************************
empty
Constructor
Purpose: (Constructs the model with no inputs, here to prevent a user from
          calling a model this way.)
*****************************************************************************/
Vector3IntegrableObject::Vector3IntegrableObject () // DO NOT USE THIS
  :
  deriv_ptr(NULL),
  dyn_body(NULL)
{
  subscribe_name = "Vector3IntegrableObject:";

  zero_arrays();

  // Current implementation requires a jeod::DynBody, making the default constructor
  // invalid.
  CMLMessage::fail (
    __FILE__, __LINE__, "Invalid Integration configuration\n",
    "Do not use the default constructor for Vector3IntegrableObject.\n"
    "It is provided for template satisfaction only.\n"
    "The jeod::DynBody must be set.\n");
}
/*****************************************************************************
single input
Constructor
Purpose: (Constructs the model with only the jeod::DynBody as an input.)
*****************************************************************************/
Vector3IntegrableObject::Vector3IntegrableObject (
     jeod::DynBody & body_in)
  :
  dyn_body(&body_in)
{
  subscribe_name = "Vector3IntegrableObject:";
  zero_arrays();
  deriv_ptr = const_cast<double *>( deriv);
  JEOD_REGISTER_CLASS (Vector3IntegrableObject);
  JEOD_REGISTER_CHECKPOINTABLE (this, integrator);
}
/*****************************************************************************
two inputs
Constructor
Purpose: (Constructs the model with the jeod::DynBody and deriv of the variable
          being integrated as inputs.)
*****************************************************************************/
Vector3IntegrableObject::Vector3IntegrableObject (
     jeod::DynBody & body_in,
     const double * deriv_ptr_in)
  :
  deriv_ptr(deriv_ptr_in),
  dyn_body(&body_in)
{
  subscribe_name = "Vector3IntegrableObject:";
  zero_arrays();

  JEOD_REGISTER_CLASS (Vector3IntegrableObject);
  JEOD_REGISTER_CHECKPOINTABLE (this, integrator);
}

/*****************************************************************************
zero_arrays
Purpose:(used in constructor to zero out the arrays.)
*****************************************************************************/
void
Vector3IntegrableObject::zero_arrays()
{
  variable[0] = 0.0;
  variable[1] = 0.0;
  variable[2] = 0.0;
  deriv[0] = 0.0;
  deriv[1] = 0.0;
  deriv[2] = 0.0;
}

/*****************************************************************************
Destructor
*****************************************************************************/
Vector3IntegrableObject::~Vector3IntegrableObject ()
{
  JEOD_DEREGISTER_CHECKPOINTABLE (this, integrator);
  destroy_integrators();
}

/*****************************************************************************
initialize
Purpose:(Initializes the Vector3 integrable object model.)
*****************************************************************************/
void
Vector3IntegrableObject::initialize()
{
  if (!enabled) {
    return;
  }

  if (deriv_ptr == NULL) {
    CMLMessage::fail (
      __FILE__, __LINE__, "Invalid Integration configuration\n",
      "The pointer to the derivatives has not been set.\n"
      "Integration of this type cannot proceed.\n");
  }
  SubscriptionBase::initialize();
  return;
}
/*****************************************************************************
activate
Purpose:(Called by subscribe() access, activates the method.)
*****************************************************************************/
void
Vector3IntegrableObject::activate()
{
  // If it doesn't know which jeod::DynBody it is attached to, it doesn't know which
  // group it is a part of.  All hope is lost.
  if (dyn_body == NULL) {
    // This is unreachable code in current implementation; dyn_body is a
    // protected pointer settable only at construction time, and there it is set
    // to the address of a passed-in reference.
    CMLMessage::error (
      __FILE__, __LINE__, "Invalid Integration configuration\n",
      "The pointer to the dyn_body has not been set.\n"
      "Integration of this type cannot proceed,\n"
      "subscription has been ignored.\n");
    return;
  }

  // If this method is run early enough, then just adding it to the jeod::DynBody
  // will suffice.  At jeod::DynBody intialization, its entire list gets added to
  // the integration group automatically.
  // If the jeod::DynBody already has an integration frame, we are too late.  It
  // must be added to the group manually.
  if (dyn_body->get_integ_frame() != nullptr) {

    jeod::DynamicsIntegrationGroup * integ_group =
                                    dyn_body->get_dynamics_integration_group();
    if (integ_group == NULL) {
      // This may be unreachable code; it suggests something has gone awry deep
      // in JEOD functionality.  A dyn-body should always have an integration
      // group.  But we must check a returned pointer from an external source
      // for the possibility that it is NULL.
      CMLMessage::error (
        __FILE__, __LINE__, "Invalid Integration configuration\n",
        "The pointer to the dyn-body's integration-group is invalid.\n"
        "Unknown error.\n"
        "subscription has been ignored.\n");
      return;
    }

    // add_integrable_object(...) ensures that it has not already been added
    // dropping an error if it has.  This is the second line of defense.  The
    // first line is this line of code can only be run if this was previously
    // inactive, which means it was removed from the integration group at that
    // time.
    integ_group->add_integrable_object(*this);
  }
  // Add it to the jeod::DynBody list of associated-integrable-objects.  This list
  // gets hauled around if the jeod::DynBody changes groups.

  dyn_body->add_integrable_object( *this);
  active = true;
  return;
}

/*****************************************************************************
deactivate
Purpose:(Called internally resulting from an unsubscribe call.)
*****************************************************************************/
void
Vector3IntegrableObject::deactivate()
{
  // If it doesn't know which jeod::DynBody it is attached to, something went
  // horribly wrong. It should never have been activated.
  if (dyn_body == NULL) {
    // This is unreachable code in current implementation; dyn_body is a
    // protected pointer settable only at construction time.  It was
    // checked at model activation, and there is no mechanism by which it could
    // have been unset.
    CMLMessage::error (
      __FILE__, __LINE__, "Invalid Integration configuration\n",
      "The pointer to the dyn_body has not been set.\n"
      "This configuration should be impossible to reach, suggesting that\n"
      "the model is being used inappropriately.\n"
      "See documentation for correct usage.\n");
    return;
  }

  // If this method is run early enough, then just adding it to the jeod::DynBody
  // will suffice.  AT jeod::DynBody intialization, its entire list gets added to
  // the integration group automatically.
  // If the jeod::DynBody already has an integration frame, we are too late.  It
  // must be added to the group manually.
  if (dyn_body->get_integ_frame() != nullptr) {

    jeod::DynamicsIntegrationGroup * integ_group =
                                    dyn_body->get_dynamics_integration_group();
    if (integ_group == NULL) {
      // This may be unreachable code; it suggests something has gone awry deep
      // in JEOD functionality.  A dyn-body should always have an integration
      // group.  But we must check a returned pointer from an external source
      // for the possibility that it is NULL.
      CMLMessage::error (
        __FILE__, __LINE__, "Invalid Integration configuration\n",
        "The pointer to the dyn-body's integration-group is invalid.\n"
        "Unknown error.\n"
        "unsubscription has been ignored.\n");
      return;
    }

    integ_group->remove_integrable_object(*this);
  }
  // Remove it to the jeod::DynBody list of associated-integrable-objects.
  dyn_body->remove_integrable_object(*this);

  active = false;
  return;
}



/*****************************************************************************
create_integrators
Purpose: (
 * Create the first order integrator for this IntegrableObject.
 * @param generator  Integrator constructor that creates the integrator.
 * @param controls   Integration controls that mediates the integrations.
 * @param time_if    Unused.
 )
*****************************************************************************/
void
Vector3IntegrableObject::create_integrators(
  const er7_utils::IntegratorConstructor & generator,
  er7_utils::IntegrationControls & controls,
  const er7_utils::TimeInterface & time_if JEOD_UNUSED)
{
  integrator.restore_manager_integrator_reference();
  integrator.create_integrator (generator, controls);
}


/*****************************************************************************
integrate
Purpose: (
 * Call the integrator to do its thing
 * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
 * @param[in]     target_stage  The stage of the integration process
 *                              that the integrator should try to attain.
 * @return The status (time advance, pass/fail status) of the integration.
   )
*****************************************************************************/
er7_utils::IntegratorResult
Vector3IntegrableObject::integrate (
  double dyn_dt,
  unsigned int target_stage)
{
  return integrator.integrate (dyn_dt, target_stage, deriv_ptr, variable);
}
