/**********************TRICK * HEADER******************************
PURPOSE: (Example of a RCS system)

PROGRAMMERS:
  (((Gary Turner) (OSR) (date) (project) (comment)))
**********************************************************************/

#include "../include/Example_RcsAdvanced.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
RcsAdvancedExample::RcsAdvancedExample()
  :
  RcsGeneric        (2), // 2 propellant-components, i.e. bi-propellant
  rcs_jets_pod      (8, num_propellant_components, time_step),
  main_engine_pod   (4, num_propellant_components, time_step),
  stringA           (num_propellant_components),
  stringB           (num_propellant_components),
  main_engine_group (num_propellant_components),
  rcs_1A( *this, rcs_jets_pod, stringA),
  rcs_2A( *this, rcs_jets_pod, stringA),
  rcs_3A( *this, rcs_jets_pod, stringA),
  rcs_4A( *this, rcs_jets_pod, stringA),
  rcs_5A( *this, rcs_jets_pod, stringA),
  rcs_6A( *this, rcs_jets_pod, stringA),
  rcs_7A( *this, rcs_jets_pod, stringA),
  rcs_8A( *this, rcs_jets_pod, stringA),
  rcs_1B( *this, rcs_jets_pod, stringB),
  rcs_2B( *this, rcs_jets_pod, stringB),
  rcs_3B( *this, rcs_jets_pod, stringB),
  rcs_4B( *this, rcs_jets_pod, stringB),
  rcs_5B( *this, rcs_jets_pod, stringB),
  rcs_6B( *this, rcs_jets_pod, stringB),
  rcs_7B( *this, rcs_jets_pod, stringB),
  rcs_8B( *this, rcs_jets_pod, stringB),
  me_1A (*this, main_engine_pod, main_engine_group),
  me_2A (*this, main_engine_pod, main_engine_group),
  me_1B (*this, main_engine_pod, main_engine_group),
  me_2B (*this, main_engine_pod, main_engine_group)
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
RcsAdvancedExample::configure_model()
{
  for (unsigned int ii = 0; ii < num_propellant_components; ++ii) {
    prop_loss_on.at(ii)  = 0.001;
    prop_loss_off.at(ii) = 0.001;
  }

  // Model behaviors
  mult_jet_flag              = true;
  self_impingement           = true;
  apply_thrust_factor_per_jet= false;
  calc_flow_rate             = false;
  input_force                = mag_and_uvec;
  imp_ref_center[0] =
  imp_ref_center[1] =
  imp_ref_center[2] =  0.1;

}

/*****************************************************************************
configure_pods
Purpose:(pod-level settings)
*****************************************************************************/
void
RcsAdvancedExample::configure_pods()
{
  prop_pods.push_back( &rcs_jets_pod);
  prop_pods.push_back( &main_engine_pod);
  // Each pod has:
  // 120N nominal thrust
  // 2000000 Pa nominal pressure
  for (unsigned int ii = 0; ii < prop_pods.size(); ++ii) {
    prop_pods.at(ii)->nominal_thrust = 120;
    prop_pods.at(ii)->pressure = 2000000;
    // diminish thrust factor by 10% per active jet:
    // thrust_factor = {1, 0.9, 0.8, 0.7, ...}
    for (unsigned int jj = 0;
         jj < prop_pods.at(ii)->get_max_num_jets_on();
         ++jj) {
      prop_pods.at(ii)->thrust_factor.push_back( 1.0 - 0.1 *ii);
    }
  }

  // The 4 main-engine jets are going to be under the group that uses the
  // mass-flow method for computing prop consumption, so each component
  // needs its flow-rate-scale-factor.
  for (unsigned int ii = 0; ii < 4 ; ++ii) {
    main_engine_pod.components[0].flow_rate_sf.push_back(1.0 - 0.05*ii);
  }
  main_engine_pod.components[1].flow_rate_sf.resize(4,1.0);
}

/*****************************************************************************
configure_groups
Purpose:(group-level settings)
*****************************************************************************/
void
RcsAdvancedExample::configure_groups()
{
  groups.push_back( &stringA);
  groups.push_back( &stringA);
  groups.push_back( &main_engine_group);

  //*********************************/
  // RCS Thrusters Model (String A) */
  //*********************************/
  stringA.signal_delay_time = 0.01;
  stringA.on_dead_time      = 0.01;
  stringA.off_dead_time     = 0.01;
  stringA.build_up_time     = 0.03;
  stringA.trail_off_time    = 0.02;
  stringA.min_on_time       = 0.04;
  stringA.min_off_time      = 0.03;
  // mixture ratio not needed because RcsGeneric::calc_flow_rate = false
  stringA.set_blow_down(true);
  // Example of blowdown profile:
  // for a pressure of 1.8MPa, the thrust is 110N, and
  // for a pressure of 2.0MPa, the thrust is 120N.
  stringA.bd_force_coef[0]    = 20.0;
  stringA.bd_force_coef[1]    = 5.0e-005;
  stringA.bd_force_coef_order = 1;
  stringA.bd_isp_coef[0]      = 240.0;
  stringA.bd_isp_coef[1]      = 2.0e-005;
  stringA.bd_isp_coef_order   = 1;
  stringA.bd_pressure_limit   = 50000;
  stringA.propc_use_isp       = true;
  stringA.isp_prop_comp_ratio.push_back(0.6);
  stringA.isp_prop_comp_ratio.push_back(0.4);

  //*********************************/
  // RCS Thrusters Model (String B) */
  //*********************************/
  stringB.signal_delay_time = 0.01;
  stringB.on_dead_time      = 0.01;
  stringB.off_dead_time     = 0.01;
  stringB.build_up_time     = 0.03;
  stringB.trail_off_time    = 0.04;
  stringB.min_on_time       = 0.05;
  stringB.min_off_time      = 0.05;
  stringB.set_blow_down(true);
  stringB.bd_force_coef[0]    = 20.0;
  stringB.bd_force_coef[1]    = 5.0e-005;
  stringB.bd_force_coef_order = 1;
  stringB.bd_isp_coef[0]      = 300.0;
  stringB.bd_isp_coef_order   = 0;
  stringB.bd_pressure_limit   = 50000;
  stringB.propc_use_isp       = true;
  stringB.isp_prop_comp_ratio.push_back(0.6);
  stringB.isp_prop_comp_ratio.push_back(0.4);

  //***********************/
  // Main Thrusters Model */
  //***********************/
  main_engine_group.signal_delay_time = 0.01;
  main_engine_group.on_dead_time      = 0.01;
  main_engine_group.off_dead_time     = 0.01;
  main_engine_group.build_up_time     = 0.05;
  main_engine_group.trail_off_time    = 0.1;
  main_engine_group.min_on_time       = 0.0;
  main_engine_group.min_off_time      = 0.2;
  main_engine_group.set_blow_down(false);
  main_engine_group.propc_use_isp     = false;  // flow rate method */
  // isp_prop_comp_ratio is irrelevant here because using flow-rate method*/
  // Mixture ratio is irrelevant because calc_flow_rate = Off */


}



/*****************************************************************************
configure_jets
Purpose:(jet-level settings)
*****************************************************************************/
void
RcsAdvancedExample::configure_jets()
{
  jets.push_back( & rcs_1A);
  jets.push_back( & rcs_2A);
  jets.push_back( & rcs_3A);
  jets.push_back( & rcs_4A);
  jets.push_back( & rcs_5A);
  jets.push_back( & rcs_6A);
  jets.push_back( & rcs_7A);
  jets.push_back( & rcs_8A);
  jets.push_back( & rcs_1B);
  jets.push_back( & rcs_2B);
  jets.push_back( & rcs_3B);
  jets.push_back( & rcs_4B);
  jets.push_back( & rcs_5B);
  jets.push_back( & rcs_6B);
  jets.push_back( & rcs_7B);
  jets.push_back( & rcs_8B);
  jets.push_back( & me_1A);
  jets.push_back( & me_2A);
  jets.push_back( & me_1B);
  jets.push_back( & me_2B);

  /****************************************/
  /* Thrusters Location (Structural frame)*/
  /****************************************/
  rcs_1A.location[0] =  0.4;
  rcs_1A.location[1] =       -2.0;
  rcs_1A.location[2] =              0.1;
  rcs_2A.location[0] =  0.6;
  rcs_2A.location[1] =       -2.0;
  rcs_2A.location[2] =              0.1;
  rcs_3A.location[0] =  0.5;
  rcs_3A.location[1] =       -2.0;
  rcs_3A.location[2] =              0.05;
  rcs_4A.location[0] =  0.5;
  rcs_4A.location[1] =       -2.0;
  rcs_4A.location[2] =              0.15;
  rcs_5A.location[0] =  0.4;
  rcs_5A.location[1] =        2.0;
  rcs_5A.location[2] =              0.1;
  rcs_6A.location[0] =  0.6;
  rcs_6A.location[1] =        2.0;
  rcs_6A.location[2] =              0.1;
  rcs_7A.location[0] =  0.5;
  rcs_7A.location[1] =        2.0;
  rcs_7A.location[2] =              0.05;
  rcs_8A.location[0] =  0.5;
  rcs_8A.location[1] =        2.0;
  rcs_8A.location[2] =              0.15;

  rcs_1B.location[0] =  0.4;
  rcs_1B.location[1] =       -2.0;
  rcs_1B.location[2] =              0.15;
  rcs_2B.location[0] =  0.6;
  rcs_2B.location[1] =       -2.0;
  rcs_2B.location[2] =              0.15;
  rcs_3B.location[0] =  0.45;
  rcs_3B.location[1] =       -2.0;
  rcs_3B.location[2] =              0.05;
  rcs_4B.location[0] =  0.45;
  rcs_4B.location[1] =       -2.0;
  rcs_4B.location[2] =              0.15;
  rcs_5B.location[0] =  0.4;
  rcs_5B.location[1] =        2.0;
  rcs_5B.location[2] =              0.05;
  rcs_6B.location[0] =  0.6;
  rcs_6B.location[1] =        2.0;
  rcs_6B.location[2] =              0.05;
  rcs_7B.location[0] =  0.45;
  rcs_7B.location[1] =        2.0;
  rcs_7B.location[2] =              0.05;
  rcs_8B.location[0] =  0.45;
  rcs_8B.location[1] =        2.0;
  rcs_8B.location[2] =              0.15;

  me_1A.location[0] = -5.0;
  me_1A.location[1] =        1.0;
  me_1A.location[2] =              0.0;
  me_2A.location[0] = -5.0;
  me_2A.location[1] =       -1.0;
  me_2A.location[2] =              0.0;
  me_1B.location[0] = -5.0;
  me_1B.location[1] =        0.0;
  me_1B.location[2] =             -1.0;
  me_2B.location[0] = -5.0;
  me_2B.location[1] =        0.0;
  me_2B.location[2] =              1.0;


  /********************/
  /* Thrust Direction */
  /********************/
  for (std::vector<RcsJet *>::iterator jet_it=jets.begin();
       jet_it != jets.end();
       ++jet_it) {
    jeod::Vector3::initialize((**jet_it).force_dir);
  }
  rcs_1A.force_dir[0]  =  1.0;
  rcs_2A.force_dir[0]  = -1.0;
  rcs_3A.force_dir[2]  =  1.0;
  rcs_4A.force_dir[2]  = -1.0;
  rcs_5A.force_dir[0]  =  1.0;
  rcs_6A.force_dir[0]  = -1.0;
  rcs_7A.force_dir[2]  =  1.0;
  rcs_8A.force_dir[2]  = -1.0;

  rcs_1B.force_dir[0]  =  1.0;
  rcs_2B.force_dir[0]  = -1.0;
  rcs_3B.force_dir[2]  =  1.0;
  rcs_4B.force_dir[2]  = -1.0;
  rcs_5B.force_dir[0]  =  1.0;
  rcs_6B.force_dir[0]  = -1.0;
  rcs_7B.force_dir[2]  =  1.0;
  rcs_8B.force_dir[2]  = -1.0;

  for( unsigned int ii = 16 ; ii < 20 ; ++ii ) { // me_*
    jets.at(ii)->force_dir[0]  = 1.0;
    // Force centerline for Main Engine jets
    // (because RcsGeneric::input_force = mag_and_uvec AND
    //          Group::blow_down        = false)
    jets.at(ii)->force_cl      = 180.0;
    // Specific impulse (Isp) for Main Engine jets
    // (because Group::blow_down = false)
    jets.at(ii)->set_isp (200.0);
    // Flow rate for Main Engine jets
    // (because Group::propc_use_isp = false and
    //          RcsGeneric::calc_flow_rate = false) */
    jets.at(ii)->set_component_flow_rate(0, 0.09);
    jets.at(ii)->set_component_flow_rate(1, 0.09);
  }

  /********************************/
  /* Set dispersions for each jet */
  /********************************/
  for( unsigned int ii = 0 ; ii < 20 ; ++ii ) {
     jets.at(ii)->error                 = RcsJet::Calc_Always;
     jets.at(ii)->force_std_dev         = 0.05;
     jets.at(ii)->force_std_mean        = 5.0;
     for( unsigned int jj = 0 ; jj < 3 ; ++jj ) {
       jets.at(ii)->force_dir_std_dev[jj]  = 0.1;
       jets.at(ii)->force_dir_std_mean[jj] = 0.1;
     }
  }

  /***************************************/
  /* Self-impingement force for each jet */
  /***************************************/

  for( unsigned int ii = 0 ; ii < 16 ; ++ii ) { // rcs_*
    for( unsigned int jj = 0 ; jj < 3 ; ++jj ) {
      jets.at(ii)->base_impingement_force[jj]    =  2.0;
      jets.at(ii)->base_impingement_torque[jj]   = 10.0;
    }
  }
  for( unsigned int ii = 16 ; ii < 20 ; ++ii ) { // me_*
    for( unsigned int jj = 0 ; jj < 3 ; ++jj ) {
      jets.at(ii)->base_impingement_force[jj]    =  5.0;
      jets.at(ii)->base_impingement_torque[jj]   = 20.0;
    }
  }
}
