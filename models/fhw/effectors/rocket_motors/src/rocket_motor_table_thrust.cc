/*******************************************************************************
PURPOSE:
   (Provide a motor model including an interpolated thrust table.)

ASSUMPTIONS:
   ((If using the PropConsumptionMburn consumption type, assumes nothing else
     is drawing mass from the same body or string.))

PROGRAMMERS:
   ((Brenton Caughron, Gary Turner) (OSR) (May 2018) (Antares) (initial)))
*******************************************************************************/

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "../include/rocket_motor_table_thrust.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RocketMotor_TableThrust::RocketMotor_TableThrust(
    DynamicMassGroup                   & mass_group,
    DynamicMassBody                    * mass_body,
    DynamicMassString                  * mass_string,
    DynamicMassBodyPropertiesInterface & mass_properties,
    const double                       & time,
    const double                       * veh_cm,
    bool                                 use_mass_string)
  :
  RocketMotor_Basic( mass_group,
                     mass_body,
                     mass_string,
                     mass_properties,
                     time,
                     veh_cm,
                     use_mass_string),
  thrust_fraction(0.0),
  thrust_max(0.0),
  table_set(),
  thrust_table(thrust_magnitude),
  isp_table(isp),
  mdot_table(mass_flow_rate),
  mburn_table(mburn),
  table_time(elapsed_time),
  isp(0.0),
  mburn(0.0),
  delta_mass(0.0),
  elapsed_time(0.0),
  consumption_type(Undefined),
  prop_mass_init(0.0)
{
  table_set.add_independent_variable(table_time);
  table_set.add_table(thrust_table);
  table_set.associate_table_and_independent();
  set_consumption_type(ConstantFlow);
}
/****************************************************************************/
RocketMotor_TableThrust::RocketMotor_TableThrust(
    DynamicMassBody & mass,
    const double    & time,
    const double    * veh_cm)
  :
  RocketMotor_TableThrust( mass_group_internal,
                           &mass,
                           NULL,
                           mass.dynamic_properties,
                           time,
                           veh_cm,
                           false)
{}
/****************************************************************************/
RocketMotor_TableThrust::RocketMotor_TableThrust(
    DynamicMassGroup & mass_group,
    DynamicMassBody  & mass,
    const double     & time,
    const double     * veh_cm)
  :
  RocketMotor_TableThrust( mass_group,
                           &mass,
                           NULL,
                           mass.dynamic_properties,
                           time,
                           veh_cm,
                           false)
{}
/****************************************************************************/
RocketMotor_TableThrust::RocketMotor_TableThrust(
    DynamicMassString & string,
    const double      & time,
    const double      * veh_cm)
  :
  RocketMotor_TableThrust( mass_group_internal,
                           NULL,
                           &string,
                           string,
                           time,
                           veh_cm,
                           true)
{}
/****************************************************************************/
RocketMotor_TableThrust::RocketMotor_TableThrust(
    DynamicMassGroup  & mass_group,
    DynamicMassString & string,
    const double      & time,
    const double      * veh_cm)
  :
  RocketMotor_TableThrust( mass_group,
                           NULL,
                           &string,
                           string,
                           time,
                           veh_cm,
                           true)
{}

/*****************************************************************************
load_thrust_data
Purpose:(Pushes the specified data onto the thrust_table and scans the data
         for its maximum value.)
*****************************************************************************/
void
RocketMotor_TableThrust::load_thrust_data(
    double * data,
    size_t   num_elements)
{
  thrust_max = 0.0; // redundant, constructor assigns = 0, just a reminder.
  for (size_t ii = 0; ii < num_elements; ++ii) {
    if (data[ii] > thrust_max) {
      thrust_max = data[ii];
    }
  }
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( num_elements);
  thrust_table.load_data( data, sizes);
}
/****************************************************************************/
void
RocketMotor_TableThrust::load_thrust_data(
    std::vector<double> & data)
{
  thrust_max = 0.0; // redundant, constructor assigns = 0, just a reminder.
  for (size_t ii = 0; ii < data.size(); ++ii) {
    if (data[ii] > thrust_max) {
      thrust_max = data[ii];
    }
  }
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( data.size());
  thrust_table.load_data( data, sizes);
}

/*****************************************************************************
load_*_data
Purpose:(These methods provide interface-consistency between the various
         required and optional tables.)
*****************************************************************************/
void
RocketMotor_TableThrust::load_isp_data(
    double * data,
    size_t   num_elements)
{
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( num_elements);
  // non-terminal NULL check in load_data, but returns false if NULL
  if (isp_table.load_data( data, sizes)) {
    // check minimum value.
    double min_value = data[0];
    for (size_t ii = 1; ii < num_elements; ++ii) {
      min_value = std::min( min_value, data[ii]);
    }
    if (min_value <= 0.0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid entry in Isp table.\n",
        "An invalid entry with value ", min_value, " was found in the Isp table.\n"
        "This could cause problems if it gets used.\n"
        "Check the entries in the Isp table.\n");
    }
  }
}
/****************************************************************************/
void
RocketMotor_TableThrust::load_isp_data(
    std::vector<double> & data)
{
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( data.size());
  if (isp_table.load_data( data, sizes)) {
    // check minimum value.
    double min_value = data[0];
    for (size_t ii = 1; ii < data.size(); ++ii) {
      min_value = std::min( min_value, data[ii]);
    }
    if (min_value <= 0.0) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid entry in Isp table.\n",
        "An invalid entry with value ", min_value, " was found in the Isp table.\n"
        "This could cause problems if it gets used.\n"
        "Check the entries in the Isp table.\n");
    }
  }
}
/****************************************************************************/
void
RocketMotor_TableThrust::load_mdot_data(
    double * data,
    size_t   num_elements)
{
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( num_elements);
  mdot_table.load_data( data, sizes);// non-terminal NULL check in load_data
}

/****************************************************************************/
void
RocketMotor_TableThrust::load_mdot_data(
    std::vector<double> & data)
{
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( data.size());
  mdot_table.load_data( data, sizes);
}

/****************************************************************************/
void
RocketMotor_TableThrust::load_mburn_data(
    double * data,
    size_t   num_elements)
{
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( num_elements);
  mburn_table.load_data( data, sizes);// non-terminal NULL check in load_data
}

/****************************************************************************/
void
RocketMotor_TableThrust::load_mburn_data(
    std::vector<double> & data)
{
  std::vector<size_t> sizes;
  sizes.push_back(1);
  sizes.push_back( data.size());
  mburn_table.load_data( data, sizes);
}

/****************************************************************************/
void
RocketMotor_TableThrust::load_time_data(
    double * data,
    size_t   num_elements)
{
  table_time.load_data( data, num_elements);
  // non-terminal NULL check in load_data
}

/****************************************************************************/
void
RocketMotor_TableThrust::load_time_data(
    std::vector<double> & data)
{
  table_time.load_data( data);
}

/*****************************************************************************
initialize
Purpose:(Initialize the rocket motor basic class, then initialize the tables
         and subscribe to them.)
*****************************************************************************/
void
RocketMotor_TableThrust::initialize()
{
  // call base-class initialize method; this sets the initialized flag.
  RocketMotor_Basic::initialize();

  table_set.initialize();
  table_set.subscribe();
}

/*****************************************************************************
update
Purpose:(The main run-time executable, executes once per frame.)
*****************************************************************************/
void
RocketMotor_TableThrust::update()
{
  // If active == false, update_status() will not be called because of C++
  // short-circuiting.
  if (!active || !update_status()) {
    return;
  }

  update_table();
  // TODO - Turner 2018/05
  //       Should the code return here if the update_table() method shuts the
  //       motor off?
  //       The update_table() method can shut down the motor in the case that
  //       the table ran out of data.
  //       There could be a return statement here --
  //          if (status == Finished) return;
  //       that would leave the model at its last good value, which could
  //       result in an underburn in the final frame.  Alternatively, the model
  //       could continue with executing an update to mass, force, and torque,
  //       which ... depending on operating choices ... could result in an
  //       overburn in the motor's final frame.
  //       On the frame immediately following shutdown (i.e. the frame after
  //       the resolution of this discussion has been applied), this line will
  //       not be seen because the status == Finished will be detected at the
  //       call to update_status() and this method will already have exited.
  //       The difference lies only in the final frame as the motor shuts down.
  //       I'm leaving it with the potential for overburn mostly for the sake
  //       of efficiency -- one less if statement to process every cycle, but
  //       leaving this comment/discussion here in case such an implementation
  //       causes a problem.

  compute_flow_rate_and_isp();

  update_mass_consumption();
  // TODO - Turner 2018/05
  //       Should the code return here if the update_mass_consumption()
  //       method shuts the motor off?
  //       See discussion above for pros/cons; in this case, the final frame
  //       is the one in which the motor runs out of propellant.

  jeod::Vector3::scale( thrust_unit_struc,
                  thrust_magnitude,
                  thrust);
  generate_torque();
}

/*****************************************************************************
set_consumption_type
Purpose:(Sets the protected consumption_type)
*****************************************************************************/
void
RocketMotor_TableThrust::set_consumption_type( ConsumptionType type_in)
{
  // Trivial case, no change.
  if (type_in == consumption_type) {
    return;
  }

  if (initialized) {
    if ( (type_in == PropConsumptionIsp &&
          !table_set.is_a_dependent_variable(isp)) ||
         (type_in == PropConsumptionMdot &&
          !table_set.is_a_dependent_variable(mass_flow_rate)) ||
         (type_in == PropConsumptionMburn &&
          !table_set.is_a_dependent_variable(mburn))) {
      CMLMessage::error(
      __FILE__,__LINE__,"Invalid initialization sequence\n",
      "Attempt to switch consumption type to ", type_in, " from ",
      consumption_type, " after initialization\n"
      "The data table for type ", type_in, " has not been loaded.\n"
      "It is not possible to switch to an empty table, nor to load data "
      "onto the necessary table after initialization.\n"
      "The instruction to change consumption type faild.\n");
      return;
    }
  }
  else {
    switch (type_in) {
     default:
       break;
     case PropConsumptionIsp:
      if (!table_set.is_a_dependent_variable(isp)) {
        table_set.add_table(isp_table);
        table_set.associate_table_and_independent();
      }
      break;
     case PropConsumptionMdot:
      if (!table_set.is_a_dependent_variable(mass_flow_rate)) {
        table_set.add_table(mdot_table);
        table_set.associate_table_and_independent();
      }
      break;
     case PropConsumptionMburn:
      if (!table_set.is_a_dependent_variable(mburn)) {
        table_set.add_table(mburn_table);
        table_set.associate_table_and_independent();
      }
      break;
    }
  }

  consumption_type = type_in;

  // Turn off all tables and then selectively turn one back on.
  table_set.enable_table_interp( &isp_table, false);
  table_set.enable_table_interp( &mdot_table, false);
  table_set.enable_table_interp( &mburn_table, false);

  switch (consumption_type) {
  case ConstantFlow:
  default:
    break;
  case PropConsumptionIsp:
    table_set.enable_table_interp( &isp_table, true);
    break;
  case PropConsumptionMdot:
    table_set.enable_table_interp( &mdot_table, true);
    break;
  case PropConsumptionMburn:
    table_set.enable_table_interp( &mburn_table, true);
    break;
  }
}

/*****************************************************************************
shutdown_motor
Purpose:(Shuts down the motor, resets output variables.)
*****************************************************************************/
void
RocketMotor_TableThrust::shutdown_motor()
{
  RocketMotor_Basic::shutdown_motor();
  thrust_fraction = 0.0;
}

/*****************************************************************************
update_table
Purpose:(Update the interp table manager and shutdown the motor if the time
         has extended past the last value in the time table)
*****************************************************************************/
void
RocketMotor_TableThrust::update_table()
{
  elapsed_time = time_now - command_time;
  table_set.update();

  if (table_time.is_off_table_back()) {
    shutdown_motor();
  }

  if (thrust_magnitude < 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Thrust Configuration \n",
      "The thrust magnitude value obtained from the thrust profile data \n "
      "is < 0, this should be an impossible value to hit. Check the thrust \n"
      "profile data to ensure it doesn't have a negative value in it. \n"
      "Resetting the thrust_magnitude value to zero. \n");
    thrust_magnitude = 0.0;
  }
  thrust_fraction = MathUtils::divide_protected( thrust_magnitude,
                                                 thrust_max,
                                                 1.0);
}

/******************************************************************************
compute_flow_rate_and_isp
Purpose:(Compute the flow rate and isp of the given motor based on the given
         consumption type)
*****************************************************************************/
void
RocketMotor_TableThrust::compute_flow_rate_and_isp()
{
  // MathUtils::divide_protected is not used because we don't want the warnings.
  switch (consumption_type) {
  case PropConsumptionIsp:
    // Isp from table, in seconds
    mass_flow_rate = MathUtils::divide_protected( thrust_magnitude,
                                                  isp * grav_sea_level,
                                                  0.0);
    break;
  case PropConsumptionMburn:
    // mburn from table, in kg
    delta_mass =  dyn_mass_properties.consumable_mass -
                  (prop_mass_init - mburn);
    if (MathUtils::is_near_equal( delta_mass, 0.0)) {
      mass_flow_rate = 0.0;
    }
    else {
      mass_flow_rate = MathUtils::divide_protected( delta_mass, dt, 0.0);
    }
    // fallthrough
  case Undefined:
  case ConstantFlow:
  case PropConsumptionMdot:
    // mass_flow_rate from table, in kg/s
    if (MathUtils::is_near_equal( thrust_magnitude, 0.0)) {
      isp = 0.0;
    }
    else {
      isp = MathUtils::divide_protected( thrust_magnitude,
                                         mass_flow_rate * grav_sea_level,
                                         0.0);
    }
  }
}

/*****************************************************************************
start_motor
Purpose:(Executes when status changes from Inactive to Firing)
*****************************************************************************/
void
RocketMotor_TableThrust::start_motor()
{
  RocketMotor_Basic::start_motor();
  prop_mass_init = dyn_mass_properties.consumable_mass;
}
