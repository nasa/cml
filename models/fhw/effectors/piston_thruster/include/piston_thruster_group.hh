/*********************GENERIC PISTON THRUSTER MODEL HEADER ***********************
PURPOSE:
  (To model the force and moments imparted on both parts of a vehicle
   during its separation when a piston thruster or a group of them fires.)

ASSUMPTIONS AND LIMITATIONS:
  ((This model provides 2 methods for computing the force --
    a constant force generated from a known delta-velocity performance, and
    a time-dependent force generated from a lookup table.)
   (For the delta-v mode, a force is computed based on the resultant delta-v
    from testing with a known mass, and assumes a square wave impulse.)
   (If a force-time model is selected,the the force is computed using a 2D
    table lookup in percentage of nominal thrust and stroke time.)
   (Friction is not considered directly, but may be introduced by reducing
    the available percentage of thrust in the table lookup.)
   (Individual thrusters may be failed off; this can also be simulated by
    reducing the available percentage of thrust in the table lookup.)
   (Although the same force magnitude is applied to both vehicles, there is
    no guarantee that this is an action-reaction pair; the direction of
    application is independent for each vehicle.)
   (Peculiar use of elevation and azimuth, from legacy code.
    Elevation is from the y-z plane towards the x-axis.
    Azimuth is measured from the negative-z axis towards the y-axis in the
    y-z plane.))

LIBRARY DEPENDENCY:
  (../src/piston_thruster_group.cc)

PROGRAMMERS:
  (((Jeff Semrau) (Honeywell) (Oct-2011) (Initial Implementation))
   ((Mark McPherson) (NASA/DM42) (Oct-2011) (Integration into ANTARES))
   ((Mark McPherson) (NASA/DM42) (May-2012) (lmbp 1650)
                             (New Thruster Load Data Table Lookup))
   ((Mark McPherson) (NASA/DM42) (June-2013) (AGDL200001040)
                             (Updates to allow jettison delta-v))
   ((Gary Turner) (OSR) (Apr 2016) (Antares-CML)
                             (genericized the model developed for Antares))
   ((Brent Caughron) (OSR) (June 2017) (Antares) (IV&V code review clean up)))
*********************************************************************************/

#ifndef PISTON_THRUSTER_GROUP_HH
#define PISTON_THRUSTER_GROUP_HH

#include <list> // For std::list
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh"

#include "piston_thruster.hh"

/*****************************************************************************
PistonThrusterGroupInputs
Purpose:(Initialization of user-specified inputs for a group of piston
         thrusters, such inputs affect the numberical value of the
         resulting effect.)
*****************************************************************************/
class PistonThrusterGroupInputs
{
public:
  // For use with ReferenceDeltaV option
  double duration;  /* (s)   Duration of the thruster stroke. */
  double deltaV;    /* (m/s) Delta-V imparted to test mass. */
  double load_mass; /* (kg)  Mass used to generate delta-v in test. */

  // For use with TableLookup
  double load_profile;                   /* (--) Thruster load percentage. */
  TableLookupSet           table_set;    /* (--) The table-manager. */
  TableIndependentVariable load_pct_table; /* (--)
               Lookup data for thruster load percentage */
  TableIndependentVariable time_table;   /* (s)   Thruster stroke time data. */
  GenericMultiInputTable   thrust_table; /* (lbf) Thruster load data. */

  PistonThrusterGroupInputs( const double & time,
                             double & thrust);
private:
  PistonThrusterGroupInputs(const PistonThrusterGroupInputs& rhs);
  PistonThrusterGroupInputs & operator = (const PistonThrusterGroupInputs& rhs);
};


/*******************************************************************************
PistonThrusterGroupParams
Purpose:(Initialization of user-specified inputs for a group of piston
         thrusters, such inputs affect the operation of the piston-thrusters;
         all thrusters in a group have identical parameters.)
*******************************************************************************/
class PistonThrusterGroupParams
{
public:
  enum ForceMethod
  {
    ReferenceDeltaV       = 1, /* Use a reference delta-v and mass. */
    TimeTableLookup       = 2, /* Use table-lookup. */
    SeparationTableLookup = 3  /* Not implemented. */
  };

  ForceMethod mode; /* (--) Thruster Force Mode. */
  double max_time;  /* (s)  Max time period the thruster can operate over. */

  PistonThrusterGroupParams()
    :
    mode(ReferenceDeltaV),
    max_time(0.0)
  {};
};


/*******************************************************************************
PistonThrusterGroup
Purpose:(Initialization of a collection of piston thrusters operating between
         the same two bodies and operating subject to the same
         controlling parameters.)
*******************************************************************************/
class PistonThrusterGroup : public SubscriptionBase
{
public:
  PistonThrusterGroupInputs input; /* (--) Inputs */
  PistonThrusterGroupParams param; /* (--) Parameters */
  PistonThrusterOutputs     out_A; /* (--) Outputs for vehicle-A */
  PistonThrusterOutputs     out_B; /* (--) Outputs for vehicle-B */

protected:
  std::list<PistonThruster *> piston_thrusters; /* (--)
        Set of piston thrusters that will fire simultaneously. */
  double force_mag;    /* (N)
        Magnitude of each of the individual thruster forces. Populated from
        either the table-lookup or the computation of
        reference-impulse / reference-time. */
  double start_time;   /* (s) Time at which firing starts. */
  const double & time; /* (s) Simulation clock. */

public:
  explicit PistonThrusterGroup( const double & time_in);
  virtual ~PistonThrusterGroup(){};

  virtual void initialize();
  virtual void update();
  void add_piston_thruster( PistonThruster & new_thruster);
  void set_mode( PistonThrusterGroupParams::ForceMethod new_mode);

protected:
  virtual void activate();
  virtual void deactivate();

private:
  PistonThrusterGroup(const PistonThrusterGroup& rhs);
  PistonThrusterGroup & operator = (const PistonThrusterGroup& rhs);
};
#endif
