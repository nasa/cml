/**********************TRICK * HEADER******************************
PURPOSE: (Example of a RCS system.  For testing purpsoes only.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2017) (Antares) (initial)))
**********************************************************************/

#include "../include/rcs_test_multigroup.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RcsTestMultigroup::RcsTestMultigroup()
  :
  RcsGeneric (2), // bi-propellant
  rcs_pod_1    (2, num_propellant_components, time_step),
  rcs_pod_2    (2, num_propellant_components, time_step),
  group_1      (num_propellant_components),
  group_2      (num_propellant_components),
  jet_01( *this, rcs_pod_1, group_1),
  jet_02( *this, rcs_pod_1, group_2),
  jet_03( *this, rcs_pod_2, group_1),
  jet_04( *this, rcs_pod_2, group_2)
{
  default_data();
}

/*****************************************************************************
default_data
Purpose:(set the default data)
*****************************************************************************/
void
RcsTestMultigroup::default_data()
{
  configure_model();
  configure_groups();
  configure_pods();
  configure_jets();
}

/*****************************************************************************
configure_model
Purpose:(model-level settings)
*****************************************************************************/
void
RcsTestMultigroup::configure_model()
{
  input_force    = RcsGeneric::vector;
}


/*****************************************************************************
configure_pods
Purpose:(pod-level settings)
*****************************************************************************/
void
RcsTestMultigroup::configure_pods()
{
  prop_pods.clear();
  prop_pods.push_back( &rcs_pod_1);
  prop_pods.push_back( &rcs_pod_2);

  rcs_pod_1.pressure = 1e6;
  rcs_pod_2.pressure = 1e6;

  rcs_pod_1.nominal_thrust = 100;
  rcs_pod_2.nominal_thrust = 100;
    
  /* Propellant Pod Paramters */
  rcs_pod_1.thrust_factor.at(0) = 1.0;
  rcs_pod_1.thrust_factor.at(1) = 0.9;
  rcs_pod_2.thrust_factor.at(0) = 1.0;
  rcs_pod_2.thrust_factor.at(1) = 0.9;

  rcs_pod_1.components.at(0).flow_rate_sf.at(0) = 1.0;
  rcs_pod_1.components.at(0).flow_rate_sf.at(1) = 0.9;
  rcs_pod_1.components.at(1).flow_rate_sf.at(0) = 1.0;
  rcs_pod_1.components.at(1).flow_rate_sf.at(1) = 0.9;
  rcs_pod_2.components.at(0).flow_rate_sf.at(0) = 1.0;
  rcs_pod_2.components.at(0).flow_rate_sf.at(1) = 0.9;
  rcs_pod_2.components.at(1).flow_rate_sf.at(0) = 1.0;
  rcs_pod_2.components.at(1).flow_rate_sf.at(1) = 0.9;
}

/*****************************************************************************
configure_groups
Purpose:(group-level settings)
*****************************************************************************/
void
RcsTestMultigroup::configure_groups()
{
  groups.clear();
  groups.push_back( &group_1);
  groups.push_back( &group_2);

  // For propellant consumption, have group 1 use Isp and group_2 use flow-rate.
  group_1.propc_use_isp = true;
  group_2.propc_use_isp = false;

  group_1.isp_prop_comp_ratio.at(0) = 0.6;
  group_1.isp_prop_comp_ratio.at(1) = 0.4;


}


/*****************************************************************************
configure_jets
Purpose:(jet-level settings)
*****************************************************************************/
void
RcsTestMultigroup::configure_jets()
{
  jets.clear();
  jets.push_back( &jet_01);
  jets.push_back( &jet_02);
  jets.push_back( &jet_03);
  jets.push_back( &jet_04);
  
  // Thruster Locations set to default [0,0,0]
  // Force Vectors (Structural Frame).  All aligned 
  jet_01.force[0] = 100.0;
  jet_02.force[0] = 100.0;
  jet_03.force[0] = 100.0;
  jet_04.force[0] = 100.0;
  
  // Thruster specific impulses
  jet_01.set_isp(200);
  jet_02.set_isp(200);
  jet_03.set_isp(200);
  jet_04.set_isp(200);
}
