/*******************************************************************************
PURPOSE:
   (Provide a motor model including an interpolated thrust table.)

ASSUMPTIONS:
   ((If using the PropConsumptionMburn consumption type, assumes nothing else
     is drawing mass from the same body or string.))

LIBRARY DEPENDENCIES:
   ((../src/rocket_motor_table_thrust.cc))

PROGRAMMERS:
   (((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares) (initial)))
 ******************************************************************************/
#ifndef CML_SOLID_ROCKET_MOTOR_TABLE_THRUST_HH
#define CML_SOLID_ROCKET_MOTOR_TABLE_THRUST_HH

#include <vector>
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"
#include "cml/models/utilities/table_interp_cpp/include/generic_multi_input_table.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_independent_variable.hh"

#include "rocket_motor_basic.hh"

class RocketMotor_TableThrust : public RocketMotor_Basic {
public:
  enum ConsumptionType {
    Undefined = -1,
    ConstantFlow = 0,
    PropConsumptionIsp = 1,
    PropConsumptionMdot = 2,
    PropConsumptionMburn = 3
  };

  double thrust_fraction; /* (--) Current thrust divided by thrust_max. */

protected:
  double thrust_max;      /* (N)  Maximum possible output thrust contribution
                                  of this motor. */

  TableLookupSet           table_set;    /* (--) Interpolation tables manager.*/
  GenericMultiInputTable   thrust_table; /* (--) Thrust-values table. Values
                                                 assume units: N. */
  GenericMultiInputTable   isp_table;    /* (--) Specific-impulse-values table.
                                                 Values assume units: s */
  GenericMultiInputTable   mdot_table;   /* (--) Mass-loss-rate-values table.
                                                 Values assume units: kg/s */
  GenericMultiInputTable   mburn_table;  /* (--) Accumulated-mass-burned table.
                                                 Values assume units kg */
  TableIndependentVariable table_time;   /* (--) Time-points table. Values
                                                 assume units: s */

  double isp;             /* (s)  Current Specific Impulse. */
  double mburn;           /* (kg) Mass burned according to the mburn-table */
  double delta_mass;      /* (kg)
       Mass removed on any given frame. This value resets every cycle.
       This value is positive if the current record of consumable mass
       is larger than the table-driven record of how much mass should remain.
       Used only in the case of consumption_type = PropConsumptionBurn. */
  double elapsed_time;    /* (s) Time since the motor started. Used as
                                 independent variable for table lookup. */
  ConsumptionType consumption_type; /* (--)   Consumption-type enumeration */
  double prop_mass_init;            /* (kg)   Initial consumable mass. */
  #ifndef SWIG
  static constexpr double grav_sea_level = 9.80665; /* (m/s2)
                                 Gravity at sea-level for Isp calc. */
  #endif

  // The public constructors all call this protected constructor, which
  // eliminates the need to have four nearly-identical initialization lists.
  // The arguments are passed through to the generic RocketMotor_Basic()
  // constructor.
  RocketMotor_TableThrust(
                    DynamicMassGroup                   & mass_group,
                    DynamicMassBody                    * mass_body,
                    DynamicMassString                  * mass_string,
                    DynamicMassBodyPropertiesInterface & mass_properties,
                    const double                       & time,
                    const double                       * veh_cm,
                    bool                                 use_mass_string);
public:
  RocketMotor_TableThrust( DynamicMassBody & mass,
                           const double    & time,
                           const double    * veh_cm);
  RocketMotor_TableThrust( DynamicMassGroup & mass_group,
                           DynamicMassBody  & mass,
                           const double     & time,
                           const double     * veh_cm);
  RocketMotor_TableThrust( DynamicMassString & string,
                           const double      & time,
                           const double      * veh_cm);
  RocketMotor_TableThrust( DynamicMassGroup  & mass_group,
                           DynamicMassString & string,
                           const double      & time,
                           const double      * veh_cm);
  virtual ~RocketMotor_TableThrust(){};

  void load_thrust_data(double * data,
                        size_t   num_elements);
  void load_thrust_data(std::vector<double> & data);
  void load_isp_data(   double * data,
                        size_t   num_elements);
  void load_isp_data(   std::vector<double> & data);
  void load_mdot_data(  double * data,
                        size_t   num_elements);
  void load_mdot_data(  std::vector<double> & data);
  void load_mburn_data( double * data,
                        size_t   num_elements);
  void load_mburn_data( std::vector<double> & data);
  void load_time_data(  double * data,
                        size_t   num_elements);
  void load_time_data(  std::vector<double> & data);

  virtual void initialize() override;
  virtual void update() override;
  void set_consumption_type( ConsumptionType);
  virtual void shutdown_motor() override;

protected:
  virtual void start_motor() override;
  void update_table();
  void compute_flow_rate_and_isp();
private:
  // Not implemented:
  RocketMotor_TableThrust (const RocketMotor_TableThrust&);
  RocketMotor_TableThrust & operator = (const RocketMotor_TableThrust&);
};
#endif
