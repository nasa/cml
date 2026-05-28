/*******************************************************************************
PURPOSE:
   (Provide a generic solid-motor model.  This class has bare-bones capability.
    This class provides no mechanism for variable thrust profiles.  Such a
    feature would have to be added as a sub-class.  See, e.g.
    RocketMotor_TableThrust for a table-driven thrust profile.)

LIBRARY DEPENDENCIES:
   ((../src/rocket_motor_basic.cc))

PROGRAMMERS:
   (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares) (initial)))
 ******************************************************************************/

#ifndef CML_SOLID_ROCKET_MOTOR_BASIC_HH
#define CML_SOLID_ROCKET_MOTOR_BASIC_HH

#include "cml/models/dynamics/mass/dynamic_mass/include/dynamic_mass_body.hh"
#include "cml/models/dynamics/mass/dynamic_mass/include/dynamic_mass_string.hh"
#include "cml/models/dynamics/mass/dynamic_mass/include/dynamic_mass_group.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "rocket_motor_dispersions.hh"


/*****************************************************************************
RocketMotor_Basic
Purpose:(Barebones rocket motor implementation)
*****************************************************************************/
class RocketMotor_Basic : public SubscriptionBase {
protected:
  // external references:
  DynamicMassGroup                   & mass_group; /* (--)
     reference to the dynamic-mass group containing the propellant mass(es)
     being accessed by this rocket-motor.  Typically, when the motor is
     firing, mass loss can be significant so it is important to update the
     mass tree at frequent intervals.  The individual dynamic-mass body
     elements are not capable of updating the entire mass tree, but the
     mass-group is.
     If no MassGroup is available, the internal MassGroup will be used.
     However, because of the potential for race-conditions between two
     groups managing the same mass, if a MassGroup exists, it should be
     passed in at construction time. */
  DynamicMassBody                    * prop_mass_body; /* (--)
     pointer to the propellant mass-body.
     When the model is used with propellant being drawn from a single tank,
     this is that tank.
     When the model is used with propellant being drawn from a string, this
     is nullptr. */
  DynamicMassString                  * prop_mass_string; /* (--)
     pointer to the propellant mass-string.
     When the model is used with propellant being drawn from a mass-string,
     this is that string.
     When the model is used with propellant being drawn from a single body,
     this is nullptr. */
  DynamicMassBodyPropertiesInterface & dyn_mass_properties; /* (--)
     reference to the propellant mass-properties.
     When the model is used with propellant being drawn from a single tank,
     this is the dynamic_properties element of that tank, i.e.
     prop_mass_body->dynamic_properties.
     When the model is used with propellant being drawn from a string, this
     is a reference to that string (DynamicMassString inherits directly from
     DynamicMassBodyPropertiesInterface). */

  const double & time_now; /* (s)   Current time during the model run */
  const double * veh_cm;   /* (--)
                            3-vec position of the vehicle-CoM in struc frm */

public:
  // Input values
  RocketMotorDispersions dispersions; /* (--) dispersion values.*/

  bool   commanded;          /* (--)   True if command sent to ignite */
  bool   force_mass_update;  /* (--)
        Flag controlling whether to force mass updates to propagate through the
        mass-tree at the time they are applied versus waiting for
        a regularly scheduled mass-group update.  If the mass-group is NOT being
        externally updated (which is likely if the internal mass-group is being
        used), then this flag should be True or the mass tree will never update.
        Default: true. */
  bool   motor_can_be_shutdown; /* (--)
        Additional control flag based on the interpretation of the commanded
        flag.  If this is false, then once commanded the motor will run until
        it exhausts its propellant
        If this is true, the motor will run until the
        command flag is set to false, at which point the motor will transition
        to status=Inactive.
        In either case, if the motor exhausts its propellant it will shutdown
        automatically with status=Finished.
        Default: false*/
  double thrust_magnitude;     /* (N)  Magnitude of thrust */
  double thrust_unit_motor[3]; /* (--) Thrust direction in the motor frame */
  double position[3];          /* (m)  Reference position of action for the
                                         motor expressed in structural frame.
                                         Used to generate moment. */
  double mass_flow_rate; /* (kg/s) Rocket mass burn rate */

  // input / output
  double T_struc_to_motor_frame[3][3]; /* (--)
            Input as the nominal transformation from struc to the motor
            frame.  Internally modified to provide the transformation after
            dispersions have been applied.*/

  // output
  double thrust[3];    /* (N)   Output thrust contribution of this motor,
                                expressed in structural frame */
  double moment[3];    /* (N*m) Moment of this motor, expressed in structural
                                frame */

  enum MotorStatus {
    Inactive, // Motor has not yet fired or has been shut down before depleting
              //     its propellant
    Firing,   // Motor is firing
    Finished  // Motor has exhausted its propellant
  };

protected:
  MotorStatus status;  /* (--)  Current status of this motor. */
  DynamicMassGroup  mass_group_internal; /* (--)
      An internally-used dynamic-mass-group, used for ensuring the changing
      mass properties are propagated at the same calling rate as this model.
      If a mass-group already exists, and the sim is running in multiple
      threads, this can produce a race condition with the existing group.
      In this situation, the existing group should be passed in at
      construction time. */
  bool   use_mass_string; /* (--)
      Indicates whether the motor is drawing mass off a single body (false)
      or a string of bodies (true).
      Set based on which constructor is used. */

  double dt;           /* (s)   Model cycle time */
  double time_last;    /* (s)   Last time update() was executed */
  double command_time; /* (s)   Time at which ignition command is received */
  double burnout_time; /* (s)   Time of burnout, when tailoff ends */

  double thrust_unit_struc[3]; /* (--) Thrust direction in the struc frame */

  // The public constructors all call this protected constructor, which
  // eliminates the need to have four nearly-identical initialization lists.
  RocketMotor_Basic(DynamicMassGroup                   & mass_group_in,
                    DynamicMassBody                    * mass_body_in,
                    DynamicMassString                  * mass_string_in,
                    DynamicMassBodyPropertiesInterface & mass_properties_in,
                    const double                       & time_in,
                    const double                       * veh_cm_in,
                    bool                                 use_mass_string_in);
public:
  RocketMotor_Basic(DynamicMassBody & mass_in,
                    const double    & time_in,
                    const double    * veh_cm_in);
  RocketMotor_Basic(DynamicMassGroup & mass_group_in,
                    DynamicMassBody  & mass_in,
                    const double     & time_in,
                    const double     * veh_cm_in);
  RocketMotor_Basic(DynamicMassString & string_in,
                    const double      & time_in,
                    const double      * veh_cm_in);
  RocketMotor_Basic(DynamicMassGroup  & mass_group,
                    DynamicMassString & string_in,
                    const double      & time_in,
                    const double      * veh_cm_in);
  virtual ~RocketMotor_Basic(){};

  virtual void initialize();
  virtual void update();
  virtual void hold_motor();
  virtual void shutdown_motor();

  double get_mass() { return dyn_mass_properties.consumable_mass; }
  MotorStatus get_status() { return status; }

protected:
  void generate_torque();
  virtual bool update_status();
  void update_mass_consumption();
  virtual void start_motor();
  #ifndef SWIG
  void activate() override;
  #endif
private:
  // Not implemented:
  RocketMotor_Basic (const RocketMotor_Basic&);
  RocketMotor_Basic & operator = (const RocketMotor_Basic&);
};
#endif