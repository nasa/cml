/*******************************************************************************
PURPOSE:
   (Provide an enhanced solid-motor model with multiple nozzles and
     optional flex effects..)

LIBRARY DEPENDENCIES:
   ((../src/rocket_motor_multi_nozzle.cc))

PROGRAMMERS:
   (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares)
          (initial, providing features dictated by requirements for
           CEV-LAS motors)))
 ******************************************************************************/

#ifndef CML_SOLID_ROCKET_MOTOR_MULTI_NOZZLE_HH
#define CML_SOLID_ROCKET_MOTOR_MULTI_NOZZLE_HH

#include "rocket_motor_table_thrust.hh"
#include "rocket_motor_nozzle.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

class RocketMotor_MultiNozzle : public RocketMotor_TableThrust {
protected:
  // external references
  const double & atmos_pressure; /* (N/m2) For back-pressure
                                           considerations */
  const double * motor_lin_flex; /* (m)    Linear flex perturbations,
                                           3*num_noz in length */
  const double * motor_rot_flex; /* (rad)  Rotational flex perturbation,
                                           3*num_noz in length */

public:
  bool table_is_net_thrust;   /* (--)
               Flag indicates that the data tables provided already account
               for cosine losses and other factors. In this case, the tabulated
               value represents the magnitude of the vector sum of the nozzle
               thrusts, not the sum of their vector magnitudes.
               Default: false. */
  bool compute_cosine_losses; /* (--)
               Flag indicating whether to compute cosine losses by accumulating
               nozzle thrust vectors.
               Valid only in the case that table_is_net_thrust = true.
               If true, code will compute the public cosine_loss_scale_factor.
               If false, code will use the input value, which defaults to 1.0.
               Default: false. */
  bool   atm_press_adjust;    /* (--)
               Adjust the thrust based on the atmospheric pressure */
  double cosine_loss_scale_factor; /* (--)
               A scale factor to convert from raw-thrust values (before cosine
               losses are included) to net-thrust values (including cosine
               losses).
               net-thrust = raw-thrust * scale-factor.
               Default 1.0. */

  // Output:
  double net_roll_torq;       /* (N*m) Roll torque */
  double thrust_vac[3];       /* (N)   Vacuum thrust vector. */
  double thrust_vac_mag;      /* (N)   Magnitude of vacuum thrust vector. */

protected:
  bool   using_flex;          /* (--)  Factor in perturbations due to
                                       flex if true */
  double flex_threshold;      /* (rad) Angle below which rotational flex
                                       perturbations are considered negligible.
                                       Default: 1.0E-12. */
  size_t num_flex_elements;   /* (--)  Number of flex elements. This must be
                                       3x larger than the number of nozzles. It
                                       is passed in at initialization as a
                                       sanity check.*/
  std::vector<RocketMotorNozzle *> nozzles_ptr_vec; /* (--)
        Vector of pointers to nozzles used by this motor.*/
  size_t num_noz;             /* (--)  Number of nozzles on motor */

  // The public constructors all call this protected constructor, which
  // eliminates the need to have four nearly-identical initialization lists.
  // The first seven arguments are passed through to the generic 
  // RocketMotor_Basic constructor.
  RocketMotor_MultiNozzle(
                    DynamicMassGroup                   & mass_group,
                    DynamicMassBody                    * mass_body,
                    DynamicMassString                  * mass_string,
                    DynamicMassBodyPropertiesInterface & mass_properties,
                    const double                       & time,
                    const double                       * veh_cm,
                    bool                                 use_mass_string,
                    const double                       & atm_press);
public:
  RocketMotor_MultiNozzle( DynamicMassBody & mass,
                           const double    & time,
                           const double    & atm_press_in,
                           const double    * veh_cm);
  RocketMotor_MultiNozzle( DynamicMassGroup & mass_group,
                           DynamicMassBody  & mass,
                           const double     & time,
                           const double     & atm_press_in,
                           const double     * veh_cm);
  RocketMotor_MultiNozzle( DynamicMassString & string,
                           const double      & time,
                           const double      & atm_press_in,
                           const double      * veh_cm);
  RocketMotor_MultiNozzle( DynamicMassGroup  & mass_group,
                           DynamicMassString & string,
                           const double      & time,
                           const double      & atm_press_in,
                           const double      * veh_cm);
  virtual ~RocketMotor_MultiNozzle(){};

  void add_nozzle( RocketMotorNozzle &);
  virtual void initialize(size_t num_flex_elements = 0,
                          const double * motor_lin_flex_in = nullptr,
                          const double * motor_rot_flex_in = nullptr);
  void force_initialize(size_t num_flex_elements = 0,
                        const double * motor_lin_flex_in = nullptr,
                        const double * motor_rot_flex_in = nullptr);
protected:
  virtual void initialize_nozzles();

public:
  virtual void update() override;
  virtual void shutdown_motor() override;

  void enable_flex();
  void disable_flex() { using_flex = false; }
  void set_flex_threshold(double new_threshold);

  size_t get_num_noz() { return num_noz; }

private:
  // Not implemented:
  RocketMotor_MultiNozzle (const RocketMotor_MultiNozzle& rhs);
  RocketMotor_MultiNozzle & operator = (const RocketMotor_MultiNozzle& rhs);
};
#endif