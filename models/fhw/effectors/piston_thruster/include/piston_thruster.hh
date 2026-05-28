/*********************GENERIC PISTON THRUSTER MODEL HEADER *********************
PURPOSE:
  (To model the force and moments imparted on opposing sides of adjacent
   vehicles that were previously connected and separated by the thrusters.)

ASSUMPTIONS AND LIMITATIONS:
  (See piston_thruster_group.hh)

LIBRARY DEPENDENCY:
  (../src/piston_thruster.cc)

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
*******************************************************************************/

#ifndef PISTON_THRUSTER_HH
#define PISTON_THRUSTER_HH

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"


/*******************************************************************************
PistonThrusterOutputs
Purpose:(Initialization of Piston Thruster outputs)
*******************************************************************************/
class PistonThrusterOutputs
{
public:
  double force[3];  /* (N)   Piston Thruster Force in struc frame. */
  double moment[3]; /* (N*m) Piston Thruster Moment in struc frame. */

  PistonThrusterOutputs(){
    force[0] = 0.0;
    force[1] = 0.0;
    force[2] = 0.0;
    moment[0] = 0.0;
    moment[1] = 0.0;
    moment[2] = 0.0;
  }
};


/*******************************************************************************
PistonThrusterParams
Purpose:(Parameters that describe the piston thruster.)
*******************************************************************************/
class PistonThrusterParams
{
public:
  double position[3]; /* (m)   Position of Piston Thruster in local Frame. */
  double elevation;   /* (rad) Elevation Angle of Piston Thruster:
                               measured from y-z plane towards x-axis. */
  double azimuth;     /* (rad) Azimuth Angle of Piston Thruster: in y-z plane,
                               measured from negative-z-axis towards y-axis. */
};


/*******************************************************************************
PistonThrusterVehicleSide
Purpose:(Class describing the interaction between the piston thruster and
         the vehicle on one end of it. A PistonThruster instance has two of
         these instances, one for each end.)
*******************************************************************************/
class PistonThrusterVehicleSide
{
public:
  PistonThrusterParams  param; /* (--) Position, elevation, and azimuth. */
  PistonThrusterOutputs out;   /* (--) Force and moment outputs, per thruster.*/

  double T_local_to_struc[3][3]; /* (--)
       Transformation matrix, from local frame to structural frame. */
  double position_local_frame_in_struc_frame[3];/* (m)
       Position of the origin of the local frame in the structural frame. */

protected:
  double trig_variable; /* (--) Intermediate value for a trig calculation. */
  double direction[3];  /* (--)
       3-vector direction, computed from azimuth and elevation. */
  double moment_arm[3]; /* (m)
       Moment-arm to point of application relative to CoM. */
  double position_struc[3];   /* (m) Point of application in structural frame.*/
  const double * position_CoM;/* (m)
       Pointer to the sim-side variable representing the center-of-mass
       of the vehicle. */

public:
  PistonThrusterVehicleSide();
  virtual ~PistonThrusterVehicleSide(){};

  void loadCoM( const double * pos_CoM);
  void initialize();
  void update( double force_mag);

private:
  PistonThrusterVehicleSide(const PistonThrusterVehicleSide& rhs);
  PistonThrusterVehicleSide & operator = (const PistonThrusterVehicleSide& rhs);
};


/*******************************************************************************
PistonThruster
Purpose:(The main instantiable component of the model; this includes the
         description of the thruster and how it interacts with the vehicles.)
*******************************************************************************/
class PistonThruster : public SubscriptionBase
{
public:
  PistonThrusterVehicleSide sideA; /* (--) One of the two vehicles. */
  PistonThrusterVehicleSide sideB; /* (--) The other of the two vehicles. */

  PistonThruster();
  PistonThruster( const double *,
                  const double *);
  virtual ~PistonThruster(){};

  void loadCoM( const double * pos_CoM_A,
                const double * pos_CoM_B);
  virtual void initialize();
  virtual void update( double force_mag);

protected:
  bool construction_complete; /* (--) Flag specifying whether the sides' CoM
                                      positions have been loaded. */
  virtual void deactivate();

private:
  PistonThruster(const PistonThruster& rhs);
  PistonThruster & operator = (const PistonThruster& rhs);
};
#endif
