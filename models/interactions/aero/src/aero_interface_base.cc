/*******************************TRICK HEADER************************************
PURPOSE: ()

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares) (New/refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include <cstring> // NULL
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/aero_interface_base.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
AeroInterfaceBase::AeroInterfaceBase( AtmosRelativeState & atmos_rel_state_in,
                                      const double & geod_alt_in)
  :
  atmos_rel_state( atmos_rel_state_in),
  geodetic_altitude( geod_alt_in),
  executive_ptr(NULL),
  disable_aero_moments(false),
  disable_aero_damping(false),
  max_atmos_altitude(3.5E6) // This is an arbitrary value; atmosphere turns
                            // off at altitudes above 3,500 km.
{
  subscribe_name = "AeroInterfaceBase:";
}

/*******************************************************************************
activate
Purpose: (Called from the subscribe() method to activate the whole process.)
*******************************************************************************/
void
AeroInterfaceBase::activate()
{
  if (active) {
    // UNREACHABLE CODE. This is a protected method called by way of
    // a subscribe() call. But the SubscriptionBase model (to which the
    // subscribe() call is directed) will only call activate() if !active.
    // There should be no other mechanism for calling this method.
    CMLMessage::warn(
      __FILE__, __LINE__, "Aero Executive activation stopped\n",
      "Model is already active. Should never see this message.\n");
    return;
  }

  if (executive_ptr == NULL) {
    // UNREACHABLE CODE. This base-class is abstract and the only 2
    // instantiable derivatives of it both set executive_ptr in their
    // constructors to be the address of a specific class instance.
    CMLMessage::error(
      __FILE__,__LINE__,"Failure to activate the aerodynamics.\n",
      "The aerodynamics executive has not been selected from the interface.\n"
      "Failed to start aerodynamics.\n");
   return;
  }

  // Subscribe to the dependencies. For each dependency, if the subscription
  // fails for any reason, the model is incomplete and cannot be marked as
  // active.

  // Atmos-rel-state subscribes the atmos-exec and planet-rel-state
  atmos_rel_state.subscribe();
  if (!atmos_rel_state.is_active()) {
    if (sub_pending > 0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Activation failed in Atmos-rel-state dependency.\n",
        "Aero Interface activation stopped because the atmosphere-relative-state"
        "\nfailed to activate, apparently during interface initialization.\n"
        "Atmosphere-relative-state is a required dependency.\n"
        "Aero Interface activation must be re-run (re-run initialization)\n"
        "after fixing the atmosphere-relative-state problem.\n");
      unsubscribe();
      initialized = false;
    }
    else {
      CMLMessage::error(
        __FILE__, __LINE__, "Activation failed in Atmos-rel-state dependency.\n",
        "Aero Interface activation stopped because the atmosphere-relative-state"
        "\nfailed to activate, apparently while trying to subscribe to a\n"
        "previously initialized instance of the Aero Interface.\n"
        "Atmosphere-relative-state is a required dependency.\n"
        "Aero Interface activation must be re-run (re-subscribe)\n"
        "after fixing the atmosphere-relative-state problem.\n");
    }
    atmos_rel_state.unsubscribe();
    return;
  }

  // Subscribe to the associated Aero-executive:
  executive_ptr->subscribe();
  if (!executive_ptr->is_active()) {
    CMLMessage::warn(
      __FILE__, __LINE__, "Environment out of domain.\n",
      "Aero Interface activation stopped because the Aero executive "
      "failed to activate.\n"
      "Aero executive is a required dependency.\n"
      "Aero Interface activation must be re-run after fixing the "
      "Aero Executive problem.\n");
    atmos_rel_state.unsubscribe();
    executive_ptr->unsubscribe();
    return;
  }

  // Send the executive's current Lref value to the atmosphere state:
  atmos_rel_state.set_reference_length(&executive_ptr->Lref);

  // Setting active now, before the sanity checks so that deactivate
  // can be called.
  active = true;

  // Sanity checks:
  // At the return from previous call, we should have an updated
  // planet-rel-state, because atmos_rel_state relies on planet_rel_state.
  // If the vehicle is at an altitude beyond the valid range of the
  // atmospheric model provides, then stop the subscription.
  if (geodetic_altitude > max_atmos_altitude) {
    CMLMessage::warn(
      __FILE__, __LINE__, "Environment out of domain.\n",
      "Aero Interface activation stopped because of altitude out of domain.\n"
      "current altitude: ", geodetic_altitude, " m\n"
      "maximum aero altitude (max_atmos_altitude): ", max_atmos_altitude, " m\n"
      "Aero Interface (and Executive) remain inactive.\n");
    // Deactivate the model; unsubscriptions are in deactivate().
    deactivate();
    return;
  }

  update();
}

/*******************************************************************************
deactivate
Purpose: (Called from the unsubscribe() method.)
*******************************************************************************/
void
AeroInterfaceBase::deactivate()
{
  executive_ptr->unsubscribe();
  atmos_rel_state.unsubscribe();

  // zero out the output values.
  output.zero_dsl();
  output.zero_torque();
  jeod::Vector3::initialize(output.force);

  SubscriptionBase::deactivate();
}