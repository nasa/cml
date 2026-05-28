/*******************************************************************************
PURPOSE:
   (Provides the capability to spontaneously create a dummy vehicle mid-sim.
    This is useful for creating copies of a vehicle to test different
    environment responses, or for applying a current vehicle state to another
    body with a different mass.)

ASSUMPTIONS AND LIMITATIONS:
    (Requires two sets of data inputs, one for the mass and one for the state.
     These data may come from a single vehicle, or from 2 different vehicles,
     but the source of those data must be available somewhere in the
     existing simulation.)

LIBRARY DEPENDENCIES:
   (../src/dummy_vehicle_launcher.cc)

PROGRAMMERS:
   (((Gary Turner) (OSR) (August 2014) (New))
    ((Gary Turner) (OSR) (March 2015) (Modification to make more generic))
   )
 ******************************************************************************/

#ifndef ANTARES_DUMMY_VEHICLE_LAUNCHER_HH
#define ANTARES_DUMMY_VEHICLE_LAUNCHER_HH

#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/dynamics/mass/include/mass.hh"
#include "jeod/models/dynamics/dyn_manager/include/dynamics_integration_group.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

class DummyVehicleLauncher
{
 public:
  bool copy_rot_state; /* (--) Optional flag allowing for the copying of the
                               rotational state.  Default is to copy the
                               translational state only; i.e. default: false. */
 protected:
  bool active; /* (--) whether the vehicle has been 'launched' into the sim */
  bool add_to_integ_group_at_launch; /* (--)
          Indicates that the model is responsible for adding the body into
          an integration group when it gets "launched" into the sim. */
  bool integ_group_specified; /* (--)
          Indicates that one of the set_* methods has been called. */

  jeod::DynBody & body; /* (--)
      The replica body.
      Note - this is a reference and not an actual instance, or an inheritance
      from jeod::DynBody, so that we can use simulation-object constructs
      designed for real vehicles (built with instances of jeod::DynBody)
      to model this vehicle after it has been created.  This model then only
      populates this exterior instance of the jeod::DynBody rather than
      providing the populated jeod::DynBody instance.*/
  const jeod::MassBody & real_mass_body;  /* (--) Data for the dummy mass */
  const jeod::DynBody & real_state_body; /* (--) Data for the dummy state */
  jeod::DynBody        * intended_integ_body; /* (--)
      Integrate with this body.  When "body" gets launched, add it to this body's
      integration group.*/
  jeod::DynamicsIntegrationGroup * intended_integ_group; /* (--) the integ group for body */

 public:
  DummyVehicleLauncher( jeod::DynBody & this_body,
                        const jeod::MassBody & mass,
                        const jeod::DynBody & state);
  DummyVehicleLauncher( jeod::DynBody & this_body,
                        const jeod::DynBody & real_body);
  virtual ~DummyVehicleLauncher(){};

  void initialize_integ_group_actions();
  void launch();
  void set_intended_integ_group(jeod::DynamicsIntegrationGroup * tgt);
  void set_intended_integ_body(jeod::DynBody * tgt);
  void set_intended_integ_body_state_body();

  bool get_active() {return active;}

 protected:
  void add_to_integ_group();
  void process_inconsistent_setup();

 private:
  // Not implemented:
  DummyVehicleLauncher (const DummyVehicleLauncher& rhs);
  DummyVehicleLauncher& operator = (const DummyVehicleLauncher& rhs);;
};
#endif
