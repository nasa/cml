/*******************************TRICK HEADER******************************
 * PURPOSE: (Sample grouping of 3 thrusters)
 *
 * PROGRAMMERS:
 *   (((Gary Turner) (OSR) (date) (project) (comment))
 *    ((Bingquan Wang) (OSR) (Sep 2017) (Antares)
 *                           (cleanup per table_interp_CPP refactoring)))
 ************************************************************************/

#include "trick/constant.h"
#include "../../include/piston_thruster_group.hh"

class SamplePistonGroup : public PistonThrusterGroup
{
 public:
  double vehA_CoM[3];
  double vehB_CoM[3];
  // Two different ways of creating a 3-thruster set, either is valid --
  // as an array and as 3 individuals:
  PistonThruster  thruster_set[3];
  PistonThruster  thrusterA;
  PistonThruster  thrusterB;
  PistonThruster  thrusterC;

  SamplePistonGroup(const double & time_in)
    :
    PistonThrusterGroup(time_in),
    // Construct the individuals here.
    thrusterA( vehA_CoM, vehB_CoM),
    thrusterB( vehA_CoM, vehB_CoM),
    thrusterC( vehA_CoM, vehB_CoM)
  {}

  void configure()
  {
    // Add the arrayed set to the group
    // Note - because it is an array, the elements could not be constructed
    // with CoM set, as the individual components were.  So need to load the
    // CoM positions first.
    for (unsigned int ii = 0; ii < 3; ii++) {
      thruster_set[ii].loadCoM( vehA_CoM, vehB_CoM);
      add_piston_thruster( thruster_set[ii]);
    }

    // Add the individuals thrusters to the group.
    add_piston_thruster( thrusterA);
    add_piston_thruster( thrusterB);
    add_piston_thruster( thrusterC);

    vehA_CoM[0] = 1.0;
    vehA_CoM[1] = 1.0;
    vehA_CoM[2] = 0.0;

    vehB_CoM[0] = 1.0;
    vehB_CoM[1] = -1.0;
    vehB_CoM[2] = 0.0;

    input.duration = 8.0;
    input.deltaV = 5.0;
    input.load_mass = 2.0;

    input.load_profile = 1.0;

    double scratch_pct[3] = {0,0.5,1.0};
    input.load_pct_table.load_data( scratch_pct,3);

    double scratch_time[6] = {0,2,4,6,8,10};
    input.time_table.load_data( scratch_time,6);

    double scratch_thrust[1][3][6] = {{{ 0, 0, 0, 0, 0, 0},
                                       {20,12, 8, 5, 2, 1},
                                       {30,18,12, 8, 3, 1}}};
    std::vector<size_t> dim_list = {1, 3, 6};
    input.thrust_table.load_data(&scratch_thrust[0][0][0], dim_list);

    input.table_set.add_table(input.thrust_table);
    input.table_set.add_independent_variable(input.load_pct_table);
    input.table_set.associate_table_and_independent();
    input.table_set.add_independent_variable(input.time_table);
    input.table_set.associate_table_and_independent();
    input.table_set.initialize();

    param.mode = PistonThrusterGroupParams::ReferenceDeltaV;
    param.max_time = 10.0;

    // Now configure the two sets of thrusters identically
    // thruster_set[0] matches thrusterA
    // thruster_set[1] matches thrusterB
    // thruster_set[2] matches thrusterC
    thruster_set[0].sideA.param.position[0] = 1.0;
    thruster_set[0].sideA.param.position[1] = 0.0;
    thruster_set[0].sideA.param.position[2] = 0.0;
    thruster_set[0].sideA.param.elevation = 90 * DTR;
    thruster_set[0].sideA.param.azimuth = 0;

    thruster_set[0].sideB.param.position[0] = 1.0;
    thruster_set[0].sideB.param.position[1] = 0.0;
    thruster_set[0].sideB.param.position[2] = 0.0;
    thruster_set[0].sideB.param.elevation = -90 * DTR;
    thruster_set[0].sideB.param.azimuth = 0;

    thruster_set[1].sideA.param.position[0] = 0.0;
    thruster_set[1].sideA.param.position[1] = 0.0;
    thruster_set[1].sideA.param.position[2] = 0.0;
    thruster_set[1].sideA.param.elevation = 0;
    thruster_set[1].sideA.param.azimuth = 90 * DTR;

    thruster_set[1].sideB.param.position[0] = 0.0;
    thruster_set[1].sideB.param.position[1] = 0.0;
    thruster_set[1].sideB.param.position[2] = 0.0;
    thruster_set[1].sideB.param.elevation = 0;
    thruster_set[1].sideB.param.azimuth = -90 * DTR;

    thruster_set[2].sideA.param.position[0] = 2.0;
    thruster_set[2].sideA.param.position[1] = 0.0;
    thruster_set[2].sideA.param.position[2] = 0.0;
    thruster_set[2].sideA.param.elevation = 0;
    thruster_set[2].sideA.param.azimuth = -180 * DTR;

    thruster_set[2].sideB.param.position[0] = 2.0;
    thruster_set[2].sideB.param.position[1] = 0.0;
    thruster_set[2].sideB.param.position[2] = 0.0;
    thruster_set[2].sideB.param.elevation = 0;
    thruster_set[2].sideB.param.azimuth = 0;

    thrusterA.sideA.param.position[0] = 1.0;
    thrusterA.sideA.param.position[1] = 0.0;
    thrusterA.sideA.param.position[2] = 0.0;
    thrusterA.sideA.param.elevation = 90 * DTR;
    thrusterA.sideA.param.azimuth = 0;

    thrusterA.sideB.param.position[0] = 1.0;
    thrusterA.sideB.param.position[1] = 0.0;
    thrusterA.sideB.param.position[2] = 0.0;
    thrusterA.sideB.param.elevation = -90 * DTR;
    thrusterA.sideB.param.azimuth = 0;

    thrusterB.sideA.param.position[0] = 0.0;
    thrusterB.sideA.param.position[1] = 0.0;
    thrusterB.sideA.param.position[2] = 0.0;
    thrusterB.sideA.param.elevation = 0;
    thrusterB.sideA.param.azimuth = 90 * DTR;

    thrusterB.sideB.param.position[0] = 0.0;
    thrusterB.sideB.param.position[1] = 0.0;
    thrusterB.sideB.param.position[2] = 0.0;
    thrusterB.sideB.param.elevation = 0;
    thrusterB.sideB.param.azimuth = -90 * DTR;

    thrusterC.sideA.param.position[0] = 2.0;
    thrusterC.sideA.param.position[1] = 0.0;
    thrusterC.sideA.param.position[2] = 0.0;
    thrusterC.sideA.param.elevation = 0;
    thrusterC.sideA.param.azimuth = -180 * DTR;

    thrusterC.sideB.param.position[0] = 2.0;
    thrusterC.sideB.param.position[1] = 0.0;
    thrusterC.sideB.param.position[2] = 0.0;
    thrusterC.sideB.param.elevation = 0;
    thrusterC.sideB.param.azimuth = 0;
  };

 private:
  SamplePistonGroup (const SamplePistonGroup&);
  SamplePistonGroup & operator = (const SamplePistonGroup&);
};
