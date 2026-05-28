/*******************************TRICK HEADER******************************
PURPOSE:
  (Simple model of a single reaction control system jet.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (April 2017) (Antares)
       (Initial object-oriented implementation)))
**********************************************************************/

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils

#include "../include/rcs_jet.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
RcsJet::RcsJet(
  RcsGeneric & rcs_system_,
  RcsPropPod & prop_pod_,
  RcsJetGroup & group_)
  :
  system( rcs_system_),
  prop_pod(prop_pod_),
  group(group_),
  time_step( system.time_step),

  isp(0.0),
  isp_g(0.0),
  g_at_earth_surface(9.80665),

  component_flow_rate( prop_pod.components.size()),
  component_consumption( prop_pod.components.size()),
  force_hat{0.0, 0.0, 0.0},
  T_str_to_case{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  force_hat_changed(true),
  force_cl_with_err(0.0),
  force_hat_with_err{0.0, 0.0, 0.0},
  cone_angle_err(0.0),
  azimuth_angle_err(0.0),

  force{0.0, 0.0, 0.0},
  error(No_Errors),
  location{0.0, 0.0, 0.0},
  force_cl(0.0),
  force_mag_std_dev(0.0),
  force_mag_bias_frac(0.0),
  direction_error(Vector),
  force_cl_err(0.0),
  force_hat_err{0.0, 0.0, 0.0},
  force_hat_std_dev{0.0, 0.0, 0.0},
  force_hat_std_mean{0.0, 0.0, 0.0},
  direction_dispersion(false),
  cone_angle_disp(0.0),
  azimuth_angle_disp(0.0),
  cone_angle_bias(0.0),
  cone_angle_std_dev(0.0),
  base_impingement_force{0.0, 0.0, 0.0},
  base_impingement_torque{0.0, 0.0, 0.0},
  failure(No_Failure),
  thrust_factor(0.0),
  status(Status_Off),
  on_com_time(0.0),
  off_com_time(0.0),
  on_com_time1(0.0),
  off_com_time1(0.0),
  time_left_in_trailoff(0.0),
  delta_time_on(0.0),
  scaled_force(0.0),
  total_delay_on(0.0),
  total_delay_off(0.0),
  commands(),
  command(false),
  nfired(0),
  sum_time(0.0),
  torque{0.0, 0.0, 0.0},
  scaled_impingement_force{0.0, 0.0, 0.0},
  scaled_impingement_torque{0.0, 0.0, 0.0}
{
  // Start the command list with an "Off":
  commands.push_back(false);
}


/*****************************************************************************
initialize
Purpose:()
*****************************************************************************/
void
RcsJet::initialize()
{
  //*******************************************************************
  // Configure how the user inputs the force for this jet
  //*******************************************************************
  switch( system.input_force ) {

  // Calculate force vector from force magnitude and unit vector.
  // This will actually be done in rcs_generic() (see explanation below)
   case RcsGeneric::mag_and_uvec:
    // if blowdown model active, calculate centerline force based
    // on tank pressure.  If user specifies a force, warn that it will
    // be overwritten by blowdown model.
    if( (group.get_blow_down()) &&
        (!MathUtils::is_near_equal(force_cl, 0.0 ))) { // i.e. non-zero
      CMLMessage::warn(
      __FILE__,__LINE__,"Incompatible configuration.\n",
      "JET: force_cl has been specified but will be overwritten by "
      "blowdown model\n");
      // There is no need to compute force_cl here (nor Isp): it
      // will be done in rcs_generic(), based on actual pressure
    }

    // Don't create force vector from unit vector and magnitude here, it will
    // be done in rcs_generic() when the jet is firing
    // (force is the jet thrust ONLY when it fires)
    break;

  // calculate force magnitude and unit vector from force vector
   case RcsGeneric::vector:
    // if blowdown requested, error (since blowdown model -- not IC -- will
    // then define force mag
    if( group.get_blow_down() ) {
      CMLMessage::warn(
      __FILE__,__LINE__,"Incompatible configuration.\n",
      "Jet force vector (input_force spec as 'vector' (2)) has been\n"
      "used to specify the initial conditions, but when used in blowdown\n"
      "mode, the model will define the magnitude of the jet force based on "
      "tank pressure.\n"
      "Using the specified force vector to specify the force direction.\n");
    }

    force_cl = jeod::Vector3::vmag(force);
    jeod::Vector3::normalize( force, force_hat);
    break;
   default:
    // Error
    CMLMessage::fail(
    __FILE__,__LINE__,"Incomplete configuration.\n",
    "\nERROR: Input force method flag not set.\n"\
    "Set input_force = mag_and_uvec (1) for magnitude and unit vector.  OR\n"\
    "Set input_force = vector (2) for force vector\n");
  }


  //**************************************************************************
  // Calculate flow rate based on thrust, specific impulse and mixture ratio *
  //**************************************************************************
  if (system.get_calc_flow_rate()) {
    // NOTE: if using group.propc_use_isp, this test is redundant.
    //       However, it is not expensive and having it here allows the
    //       switch back to using mass-flow at a later time.
    compute_component_flow_rates();
  }

  //**************************************************************************
  // Check random error configuration
  //**************************************************************************
  if ( (error == Calc_Fire || error == Calc_Always) &&
       (MathUtils::is_near_equal(force_mag_std_dev, 0.0)) &&
       (MathUtils::is_near_equal(jeod::Vector3::vmag(force_hat_std_dev), 0.0 )) &&
       (MathUtils::is_near_equal(cone_angle_bias, 0.0 )) &&
       (MathUtils::is_near_equal(cone_angle_std_dev, 0.0 ))){
       // NOTE - legitimate comparisons against zero; basically testing whether
       // these values have moved from their construction values.
     CMLMessage::warn(
      __FILE__,__LINE__,"Incomplete configuration.\n",
      "jet has random errors on but none of the constraining parameters "
      "are set.\n");
  }

  if (direction_dispersion) {
    apply_direction_dispersion();
  }

  //**************************************************************************
  // Initialize times based on the group
  //**************************************************************************
  off_com_time  =
  off_com_time1 = group.min_off_time;

  total_delay_on  = group.delay_time_on;
  total_delay_off = group.delay_time_off;
}


/*****************************************************************************
compute_component_flow_rates
Purpose:(Computes the nominal flow rates for each component through this jet.)
*****************************************************************************/
void
RcsJet::compute_component_flow_rates()
{
  // If in blow-down mode, this computation will be deferred;
  // actual flow will be computed based  on pressure as part of the regular
  // update, so no flow_rate is pre-calculated.
  if (group.get_blow_down()) {
    return;
  }

  if (isp_g <= 0.0 ) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid configuration.\n",
    "Invaid information when trying calculate flow rate:\n"\
    "Specific Impulse = ", isp, " s\nwith g included, velocity = ", isp_g, " m/s\n");
  }

  unsigned int num_prop_components = group.get_num_prop_components();
  double flow_rate = force_cl / isp_g;

  if (num_prop_components == 1) {
    // compute simple fuel flow, all flow on one channel
    component_flow_rate.at(0) = flow_rate;
  }
  else if (num_prop_components ==2) {  // for bi-propellant
    // Verify that the necessary variables have been set.
    // These default to 0.0, so if they are still 0.0, they have not been
    // set.
    if (group.mixture_ratio <= 0.0) {
      CMLMessage::fail(
      __FILE__,__LINE__,"Invalid configuration.\n",
      "Invaid information when trying calculate flow rate:\n"\
      "Mixture Ratio = ", group.mixture_ratio, "\n");
    }
    // Divide flow between two channels, {(X/1+X), (1/1+X)}
    component_flow_rate.at(1) = flow_rate / (1 + group.mixture_ratio);
    component_flow_rate.at(0) = component_flow_rate.at(1) *
                                              group.mixture_ratio;
  }
  else {
    // Protect against excessive # of propellant components.
    // This should have been caught in RcsGeneric::initialize,
    // but in case the configuration changed mid-sim:
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid configuration.\n",
    "'calc_flow_rate = true' assumes 1 or 2 prop components,\n"
    "but there are ", num_prop_components, " components\n");
  }
}


/*****************************************************************************
update
Purpose:(Main executable for the RcsJet class)
*****************************************************************************/
void
RcsJet::update(
   bool new_command)
{
  // Reset delta_time_on.
  // delta_time_on is how much "effective" time the jet will be on in this
  // cycle. "Effective" means that the same amount of momentum will be
  // imparted on the vehicle if the jet was on at its steady state value for
  // delta_time_on as would be imparted if the jet was on for the actual time
  // with the actual thrust level.
  // During build_up and trail_off the force is assumed to have a constant slope */
  delta_time_on = 0.0;

  for (unsigned int ii=0; ii<component_consumption.size(); ii++) {
     component_consumption.at(ii) = 0.0;
  }

  //****************************/
  // Set command
  //****************************/
  switch(failure) {
   case No_Failure:
    /*****************************/
    /* Buffer commands if needed */
    /*****************************/
    // The number of commands to be buffered is set in the initialization
    // and is equal to the ((signal_delay_time + dead time) / step size)
    // rounded down to the nearest integer.
    // Note that there is a separate dead_time for on commands and off commands
    if( group.buffer_flag ) { /* Commands need to be buffered */
      // If the new command does not match with the last element in the buffer
      // list (i.e. the previous command), need to buffer this new command into
      // the list.
      if (new_command != commands.back()) {
        if (new_command) {
          // append "Off" commands until the buffer array is sufficiently large
          // to accommodate the buffering necessary before the "On" command
          // gets processed.
          commands.resize( group.buffer_on_size, false);
        }
        else {
          // append "On" commands until the buffer array is sufficiently large
          // to accommodate the buffering necessary before the "Off" command
          // gets processed.
          commands.resize( group.buffer_off_size, true);
        }
        commands.push_back( new_command);
      }

      // Set the current command to be executed */
      command = commands.front();
      // Pop that command off the list unless it is the last one.
      if (commands.size() > 1) {
        commands.pop_front();
      }
    }
    else { // No buffering required
      command = new_command;
    }
    break;
   case Failed_On:
    command = true;
    break;
   case Failed_Off:
    command = false;
    break;
   default:
    CMLMessage::error(
    __FILE__,__LINE__,"Invalid setting.\n",
    "Illegal value of failure flag.\n");
    command = false;
  }
  //********************************************
  //Check that mass exists and pod is not failed
  //********************************************
  command = command &&
            prop_pod.is_healthy() &&
            prop_pod.mass_available();

  //**************************************************************/
  // If Blowdown model used, calculate centerline Thrust and ISP */
  //**************************************************************/
  if (group.get_blow_down()) {
    blow_down();
  }

  //***************************************/
  // Update jet state to the current time */
  //***************************************/
  // Status_On, Status_Off, Status_BuildUp, Status_TrailOff */
  update_jet_status();

  prop_pod.equiv_momentum += delta_time_on * force_cl;
}



/*****************************************************************************
blow_down
REFERENCE:
    ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
      Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
     (Trick code - rcs_generic.c by Willian Othon/LinCom/93))
ASSUMPTIONS AND LIMITATIONS:
    ((1. The blowdown force equations in the RCS model depend on tank )
     (pressure provided outside the RCS model by the blowdown tank model)
     (2. The blowdown implementation does not take into account the random)
     (errors in thrust level when determining force)
     (These errors are added later on))
Purpose: (To functionally model thrust reduction due to propellant tank
          blowdown characteristics))
*****************************************************************************/
void
RcsJet::blow_down()
{
  if (prop_pod.pressure > group.bd_pressure_limit) {
    force_cl = MathUtils::polynomial( prop_pod.pressure,
                                      group.bd_force_coef);
    set_isp( MathUtils::polynomial( prop_pod.pressure,
                                    group.bd_isp_coef));

    // Calculate flow rate, if enabled
    // Note: execution of RcsJetGroup::blow_down_invalid already ensured
    // it is a mono-propellant
    if (system.get_calc_flow_rate() && isp_g > 0) {
      component_flow_rate[0] = force_cl / isp_g;
    }
  }
  else {
    force_cl = 0.0;
    set_isp(0.0);
  }
}


/*****************************************************************************
update_jet_status
Purpose:(
    (To update each jet status : On, Off, BuildUp, TrailOff and calculate
    the equivalent on time for each jet)
REFERENCE:
    ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
      Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
     (Trick code - rcs_generic.c by Willian Othon/LinCom/93)
     (CML code - update_jet_status.c by Canton(2006) and Stachowiak(2007)
     )
ASSUMPTIONS AND LIMITATIONS:
    ((1.Delay time, Build up time, and Trail off time are all constant)
     (2.When determining jet on-time, it is assumed that force builds up)
     (  linearly to a steady state value. Then, when the jet is commanded off,)
     (  the force trails off linearly.)
     (3.The potential effect of blowdown and multiple jets firing from a single)
     (  tank is incorporated after on-time has been determined)
     (4.The jet can be commanded off during the build up and on during the trail)
     (   off unless the user defined mini¬mum on or minimum off times are violated))
PROGRAMMERS:
    ((Remi Canton)     (OSR)      (Oct.06)    (RDLaa09442)
                                               (Rewrite of rcs_generic.c)
    (Susan Stachowiak) (NASA-JSC) (June 2007) (RDLaa09540)
                                        (Min on/off time implementation))
*****************************************************************************/
void
RcsJet::update_jet_status()
{
  // Adjusts jet on and off within-frame delay times to delay jet cycling
  // when minimum on and off times constraints have not been met.

  /*******************************/
  /*Minimum off time enforcement */
  /*******************************/

  /*Check for new jet firing */
  if( command && (status == Status_Off || status == Status_TrailOff )){
    total_delay_off = group.delay_time_off;
    total_delay_on  =  group.delay_time_on;

    // Ensure that off time constraint has been met
    // If the minimum off time has not been met, add what remains to the
    // on-delay.  If that carries the on-delay across the timestep boundary,
    // turn the jet off for this time-step and leave the on-delay at the group
    // value for the next cycle
    // FIXME Turner 05/2017
    //      - this seems wrong.  Shouldn't the delay in the next cycle be the
    //         part that remains after subtracting off the full timestep.
    //         However, this is the way the algorithm was, and without a change
    //         it will be overwritten anyway when we get back to this point
    //         next time.
    if (group.min_off_time > off_com_time1 ) {
      /*off time constraint not met - add remaining off-time to delay time */
      total_delay_on  +=  (group.min_off_time - off_com_time1);
      if( total_delay_on > time_step){
         command = false;
         total_delay_on = group.delay_time_on;
      }
    }
  }
  else if (status == Status_On){
    total_delay_on = group.delay_time_on;
  }

  /******************************/
  /*Minimum on time enforcement */
  /******************************/

  /*Check for jet turning off */
  if( !command && (status == Status_On || status == Status_BuildUp)){
    total_delay_off  =  group.delay_time_off;
    total_delay_on = group.delay_time_on;
    // Determine if on time constraint met. See comment above for decription
    if( group.min_on_time > on_com_time1){
       /*On time constraint not met - add remaining off-time to delay time */
       total_delay_off +=  (group.min_on_time - on_com_time1);
      if(total_delay_off > time_step){
        command = true;
        total_delay_off = group.delay_time_off;
      }
    }
  }
  else if(status == Status_Off){
    total_delay_off = group.delay_time_off;
  }


  switch ( status ) {
  /**************/
  /* Status_Off */
  /**************/
   case Status_Off :
    if ( command ) {
      nfired += 1 ;      /* New firing */
      if (error == Calc_Fire ) {
        /****************************************************************/
        /* Errors in magnitude and direction added only when new firing */
        /****************************************************************/
        disperse_force();
      }

      /****************************************************/
      /* Compute equivalent on time and update jet status */
      /****************************************************/
      // "equivalent on time" is time needed to produce the
      //    same amount of thrust if the jet was fully on */
      if (total_delay_on + group.build_up_time > time_step ) {
        // Build up does not complete.
        // Area of the partial build-up triangle = (1/2) (base) (height)
        //   = (1/2) (dt) * (dt * slope)
        //  where dt = time over which ramp runs, time_step - total_delay_on
        //    and slope = 1/ build-up-time.
        delta_time_on = (time_step - total_delay_on) *
          (time_step - total_delay_on) / (2*group.build_up_time);
        /* Update status */
        switch_status(Status_BuildUp);
      }
      else {
        // Build up does complete.
        // area under curve = whole block - everything for the delay period -
        //                    (1/2) of the build-up component
        delta_time_on = time_step - total_delay_on - group.build_up_time/2;
        /* Update status */
        switch_status(Status_On);
      }
    } /* else stay in Status_Off */
    break;

  /*******************/
  /* Status_BuildUp */
  /*******************/
   case Status_BuildUp :
    /****************************************************/
    /* Compute equivalent on time and update jet status */
    /****************************************************/
    // "equivalent on time" is time needed to produce the
    //    same amount of thrust if the jet was fully on
    if ( command ) {
      if ( on_com_time + time_step < total_delay_on + group.build_up_time ) {
        // Build up still does not complete
        // divide area into a rectangle under a triangle
        // rectangle area = height * base =
        //         ((on-com-time - total_delay_on) * slope)  * dt
        // triangle area = (1/2) * height * base = (1/2) * (slope * dt) * dt
        delta_time_on =
           (on_com_time - total_delay_on + 0.5 * time_step) * time_step  /
                                                      group.build_up_time;
        // Stay in Status_BuildUp
      }
      else {
        // Build up does complete
        // area is complete block minus upper left triangle, which has
        //   height: (1 - (on-com-time - total_delay_on) * slope)
        //   base:   build-up-time - (on-com-time - total_delay_on)
        // A = dt - (build-up-time + (total_delay_on - on_com_time))^2 / 2 * b-u-t
        delta_time_on = time_step -
          (group.build_up_time + total_delay_on - on_com_time) *
          (group.build_up_time + total_delay_on - on_com_time) /
          (2*group.build_up_time);
        /* Update status */
        switch_status(Status_On);
      }
    }
    else { /* command is OFF: Trail off starts */
      if (on_com_time - total_delay_on + total_delay_off < group.build_up_time) {
        /* Build up does NOT complete before trail off starts */
        if ((total_delay_off +
            (on_com_time - total_delay_on + total_delay_off) *
            (group.trail_off_time / group.build_up_time)) < time_step) {
          /* Trail off completes this time step */
          delta_time_on =
            (on_com_time - total_delay_on) * (total_delay_off/group.build_up_time) +
            (total_delay_off*total_delay_off) / (2*group.build_up_time) +
            group.trail_off_time *
              (on_com_time - total_delay_on + total_delay_off) *
              (on_com_time - total_delay_on + total_delay_off) /
              (2*group.build_up_time*group.build_up_time);
          /* Update status */
          switch_status(Status_Off);
        }
        else {
          /* Trail off does NOT complete this time step */
          delta_time_on =
            (on_com_time - total_delay_on) * total_delay_off /
               group.build_up_time +
            total_delay_off * total_delay_off / (2 * group.build_up_time) +
            (time_step - total_delay_off) *
              (on_com_time - total_delay_on + total_delay_off) /
              group.build_up_time -
            (time_step - total_delay_off) * (time_step - total_delay_off) /
              (2 * group.trail_off_time);
          /* Update status */
          switch_status(Status_TrailOff);
          /* Compute time left in trail off */
          time_left_in_trailoff =
            (group.trail_off_time / group.build_up_time) *
              (on_com_time - total_delay_on + total_delay_off) -
            time_step + total_delay_off;
        }
      } else {
        /* Build up does complete before trail off starts **/
        if (total_delay_off + group.trail_off_time < time_step) {
          /* Trail off completes this time step */
          delta_time_on =
            (total_delay_on + group.build_up_time - on_com_time) *
              (1 - (total_delay_on + group.build_up_time - on_com_time) /
                   (2 * group.build_up_time)) +
            on_com_time - group.build_up_time -
            total_delay_on + total_delay_off + group.trail_off_time/2;
          /* Update status */
          switch_status(Status_Off);
        }
        else {
          /* Trail off does NOT complete this time step */
          delta_time_on =
            (total_delay_on + group.build_up_time - on_com_time) *
              (1- (total_delay_on + group.build_up_time - on_com_time) /
                  (2*group.build_up_time)) +
            on_com_time - group.build_up_time + time_step - total_delay_on -
            (time_step - total_delay_off) * (time_step - total_delay_off) /
              (2 * group.trail_off_time);
          /* Update status */
          switch_status(Status_TrailOff);
          /* Compute time left in trail off */
          time_left_in_trailoff = group.trail_off_time - time_step + total_delay_off;
        }
      }
    }
    break;

  /*************/
  /* Status_On */
  /*************/
   case Status_On :
    /****************************************************/
    /* Compute equivalent on time and update jet status */
    /****************************************************/
    // "equivalent on time" is time needed to produce the
    //   same amount of thrust if the jet was fully on
    if ( command ) {
      delta_time_on = time_step;
      /* Stay in Status_On */
    }
    else{ /* Command is OFF */
      if ( group.trail_off_time + total_delay_off > time_step ) {
        /* Trail off does not complete */
        delta_time_on = time_step -
          ((time_step - total_delay_off) * (time_step - total_delay_off)) /
          (2 * group.trail_off_time);
        /* Update status */
        switch_status(Status_TrailOff);
        /* Compute time left in trail off */
        time_left_in_trailoff =
            group.trail_off_time - time_step + total_delay_off;
      }
      else {
        /* Trail off does complete */
        delta_time_on = total_delay_off + group.trail_off_time/2;
        /* Update status */
        switch_status(Status_Off);
      }
    }
    break;

  /********************/
  /* Status_TrailOff */
  /********************/
   case Status_TrailOff :
    /****************************************************/
    /* Compute equivalent on time and update jet status */
    /****************************************************/
    // "equivalent on time" is time needed to produce the
    //  same amount of thrust if the jet was fully on
    if ( !command ) {
      if ( time_left_in_trailoff < time_step ) {
        /* Trail off does complete */
        delta_time_on = (time_left_in_trailoff * time_left_in_trailoff) /
                        (2 * group.trail_off_time);
        /* Update status */
        switch_status(Status_Off);
        /* Reset time left in trail off */
        time_left_in_trailoff = 0.0;
      } else {
        /* Trail off does NOT complete */
        delta_time_on =
           time_left_in_trailoff * time_step / group.trail_off_time -
           time_step * time_step / (2*group.trail_off_time);
        /* Update time left in trail off */
        time_left_in_trailoff -= time_step;
        /* Stay in Status_TrailOff */
      }
    }
    else{ /* Jet is commanded ON */
      /*************************/
      /* Check for min_off_time */
      /*************************/
      if (time_left_in_trailoff < total_delay_off) {
        /* Trail off does complete before build up starts */
        if (total_delay_off + group.build_up_time > time_step) {
          /* Build up does NOT complete */
          delta_time_on =
            (time_left_in_trailoff * time_left_in_trailoff) /
              (2 * group.trail_off_time) +
            ((time_step - total_delay_on) * (time_step - total_delay_on)) /
              (2 * group.build_up_time);
          /* Update status */
          switch_status(Status_BuildUp);
          /* Reset time left in trail off */
          time_left_in_trailoff = 0.0;
        } else {
          /* Build up does complete */
          delta_time_on =
            (time_left_in_trailoff * time_left_in_trailoff) /
              (2 * group.trail_off_time) +
            time_step - total_delay_on - group.build_up_time/2;
          /* Update status */
          switch_status(Status_On);
          /* Reset time left in trail off */
          time_left_in_trailoff = 0.0;
        }
      } else {
        /* Trail off does NOT complete before build up starts */
        if ( (total_delay_on + group.build_up_time -
              (group.build_up_time / group.trail_off_time)*
               (time_left_in_trailoff - total_delay_on)) > time_step) {
          /* Build up does NOT complete */
          delta_time_on =
            (total_delay_on * total_delay_on / 2 +
              (time_left_in_trailoff - total_delay_on) * time_step) /
              group.trail_off_time +
            (time_step - total_delay_on) * (time_step - total_delay_on) /
              (2 * group.build_up_time);
          /* Update status */
          switch_status(Status_BuildUp);
        }
        else {
          /* Build up does complete */
          delta_time_on =
            ( time_left_in_trailoff * total_delay_on -
              total_delay_on * total_delay_on / 2 +
              time_left_in_trailoff * group.build_up_time -
              total_delay_on * group.build_up_time +
              ( -time_left_in_trailoff * time_left_in_trailoff *
                  group.build_up_time +
                2 * time_left_in_trailoff * total_delay_on *
                  group.build_up_time -
                total_delay_on * total_delay_on * group.build_up_time) /
              (2 * group.trail_off_time)) /
             group.trail_off_time +
            time_step - total_delay_on - group.build_up_time / 2;
          /* Update status */
          switch_status(Status_On);
        }
        /* Adjust on_com_time to account for burn not starting at thrust = 0 */
        on_com_time = (time_left_in_trailoff - total_delay_on) *
          (group.build_up_time / group.trail_off_time);
        /* Reset time left in trail off */
        time_left_in_trailoff = 0.0;
      }
    }
    break;
  }


  // Increment on_com_time and effective on_com_time for each jet
  //  - on_com_time is used for build-Up and trail-off loops
  //  - effective-on-com-time (on_com_time1) is used for min on and min off
  //    calculations)
  if (status == Status_Off || status == Status_TrailOff) {
    on_com_time  = 0.0;
    on_com_time1 = 0.0;
    off_com_time  += time_step;
    off_com_time1 += time_step - delta_time_on;
  } else {
    on_com_time  += time_step;
    on_com_time1 += delta_time_on;
    off_com_time  = 0.0;
    off_com_time1 = 0.0;
  }
}


/*****************************************************************************
compute_jet_forces
PURPOSE:
    ((To calculate the forces and torques imparted on the vehicle by the jets
      and the mass demanded.))

REFERENCE:
    ((Trick code - rcs_orbiter.c by John Whynott/McDonnell Douglas/91,
      Robert Bailey/LinCom/87, Douglas Hamilton/RSOC/95)
     (Trick code - rcs_generic.c by Willian Othon/LinCom/93)
     (CML code rcs_force_fuel_calc.c by Canton & Lanoix, OSR, 2006-7))
ASSUMPTIONS AND LIMITATIONS:
    ((1.All input jet force, torque and location vectors are referenced )
     ( to the Vehicle Structural Frame. )
     (2.All force and torque outputs are centered at the body CM,)
     ( but are referenced to the Vehicle Structural Frame.))
*****************************************************************************/
void
RcsJet::compute_jet_forces()
{
  if (delta_time_on <= 0.0) {
    jeod::Vector3::initialize(force);
    jeod::Vector3::initialize(torque);
    // Do not really need to set scaled_force to 0.0, but doing so
    // helps with logging.
    scaled_force = 0.0;
    return;
  }

  // else, this jet has some on-time.  Accumulate it:
  sum_time += delta_time_on;
  system.sum_time += delta_time_on;

  // Scale the force magnitude to account for error, degradation, and timing:
  // Start with the nominal force, force_cl and modify that.

  // Apply dispersions if necessary.  Note that in the case Calc_Fire, the
  // force_cl_err was computed based on force_cl, so should be added here before
  // it gets scaled:
  switch (error) {
   case Calc_Always: // generate force_cl_err and force_hat_err every pass
    disperse_force();
    // fall-through:
    [[fallthrough]];
   case Calc_Fire:
    scaled_force = force_cl_with_err;
    break;
   case Input_Errors:
    scaled_force = force_cl + force_cl_err;
    break;
   default:
    scaled_force = force_cl;
    break;
  }
  // Apply thrust degradation resulting from having multiple thrusters
  // simultaneously running off this pod:
  scaled_force *= prop_pod.get_thrust_factor();
  // Apply thrust factor per jet as provided by an external model (if available)
  if ( system.apply_thrust_factor_per_jet ) {
    scaled_force *= thrust_factor;
  }
  // Scale by the fraction of the timestep for which the jet is on.  Scaling the
  // force here and integrating that force for the whole timestep will provide
  // an equivalent impulse.
  scaled_force *= delta_time_on / time_step;


  // Corrupt the direction to account for error and scale by magnitude
  // to generate the final force:
  switch (error) {
   case Input_Errors:
    apply_direction_error();
    // fall-through
   case Calc_Always: // already applied in disperse_force()
   case Calc_Fire:
    jeod::Vector3::scale( force_hat_with_err,
                          scaled_force,
                          force);
    break;
   default:
    jeod::Vector3::scale( force_hat,
                          scaled_force,
                          force);
    break;
  }
  /*****************************************************************/
  /* Determine torque about Body CG referenced to structural frame */
  /*****************************************************************/
  double moment_arm[3];
  jeod::Vector3::diff( location , system.get_cm(), moment_arm );
  jeod::Vector3::cross( moment_arm , force, torque );
}


/*****************************************************************************
compute_prop_consumption
Purpose:(compute the prop consumption for this jet.)
Assumptions:(component_consumption was reset to zero at the start of the cycle
             and may have been incremented by the update_status method at
             start-up and shut-down of the jet.  This computation adds to those
             values according to the actual demand.)
Note: was lines 102- in rcs_force_fuel_calc.c
*****************************************************************************/
void
RcsJet::compute_prop_consumption()
{
  if (delta_time_on <= 0.0) {
    return;
  }


  if( group.propc_use_isp) {/* use ISP */
    // calculate mass consumed based on Isp calculated from pressure
    // No mass flow if isp is zero: this happens when pressure is low
    // Note: in that case, force_cl would be zero too
    if (isp > 0.0) {
      for (unsigned int ii = 0; ii < component_consumption.size(); ++ii) {
        component_consumption[ii] +=
                         (force_cl / (isp_g)) *
                         group.isp_prop_comp_ratio.at(ii) * delta_time_on;
      }
    }
  }
  else { // use mass-flow
    if (prop_pod.num_jets_on > 0) {
      for (unsigned int ii = 0; ii < component_consumption.size(); ++ii) {
         component_consumption[ii] += prop_pod.get_flow_rate_scale_factor(ii) *
                                    component_flow_rate.at(ii) * delta_time_on;
      }
    }
  }

  // Add this jet's prop consumption (per component) to the pod
  prop_pod.increment_mass_consumption( component_consumption);
}

/*****************************************************************************
get_force_direction
Purpose:(Returns force_hat)
*****************************************************************************/
void RcsJet::get_force_direction(double force_dir[3])
{
  jeod::Vector3::copy(force_hat, force_dir);
}

/*****************************************************************************
get_component_consumption
Purpose:(returns the value of the component_consumption vector at the
         specified index)
*****************************************************************************/
double
RcsJet::get_component_consumption(
      unsigned int ix)
{
  if (ix > component_consumption.size()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Request error\n",
      "Cannot extract the index ", ix, " because component_consumption has "
      "only ", component_consumption.size(), " elements.\n");
    return 0.0;
  }
  return component_consumption.at(ix);
}

/*****************************************************************************
get_component_flow_rate
Purpose:(returns the value of the component_flow_rate vector at the
         specified index)
*****************************************************************************/
double
RcsJet::get_component_flow_rate(
      unsigned int ix)
{
  if (ix > component_flow_rate.size()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Request error\n",
      "Cannot extract the index ", ix, " because component_flow_rate has "
      "only ", component_flow_rate.size(), " elements.\n");
    return 0.0;
  }
  return component_flow_rate.at(ix);
}

/*****************************************************************************
set_isp
Purpose:(Sets the protected isp value and its isp_g partner)
*****************************************************************************/
void
RcsJet::set_isp( double isp_)
{
  isp = isp_;
  isp_g = isp * g_at_earth_surface;
}

/*****************************************************************************
set_component_flow_rate
Purpose:(Sets the component flow rate for the specified index)
*****************************************************************************/
void
RcsJet::set_component_flow_rate(
    unsigned int component_ix,
    double rate)
{
  if (component_ix > component_flow_rate.size()) {
    CMLMessage::error(
    __FILE__,__LINE__,"Assignment error\n",
    "Cannot assign a component flow-rate to component index ", component_ix, " because\n"
    "there are only ", component_flow_rate.size(), " components (so max index is ", component_flow_rate.size()-1, ").\n");
    return;
  }
  component_flow_rate.at(component_ix) = rate;
}

/*****************************************************************************
disperse_force
Purpose:(Adds errors to the direction and axial magnitude of the forces)
*****************************************************************************/
void
RcsJet::disperse_force()
{
  // thrust magnitude error
  force_cl_err = force_cl * (force_mag_bias_frac +
                        system.normal(system.generator) * force_mag_std_dev);
  force_cl_with_err = force_cl + force_cl_err;

  // Determine thrust direction error
  if (direction_error == Vector) {
    for (unsigned int ii = 0; ii< 3; ++ii) {
      force_hat_err[ii] = force_hat_std_mean[ii] +
                        system.normal(system.generator) * force_hat_std_dev[ii];
    }
  }
  else {
    // Disperse force direction (axial method)
    // First obtain the deviation angle from the principal axis:
    cone_angle_err = cone_angle_bias +
                     system.normal(system.generator) * cone_angle_std_dev;

    // Next obtain a random orientation about the principal axis in which to
    // apply the deviation angle.
    azimuth_angle_err = system.uniform(system.generator) * M_PI;
  }
  apply_direction_error();
}

/*****************************************************************************
generate_force_direction_matrix
Purpose:(generates the transformation matrix based on the force_hat vector)
*****************************************************************************/
void
RcsJet::generate_force_direction_matrix()
{
  // In event that force_hat lies along +x structural axis, align case and
  // structural axes:
  if (MathUtils::is_near_equal(force_hat[0], 1.0)) {
    jeod::Matrix3x3::identity(T_str_to_case);
    return;
  }
  // In event that force_hat lies along -x structural axis, set
  // case y-axis = structural z-axis and vice versa.
  if (MathUtils::is_near_equal(force_hat[0], -1.0)) {
    jeod::Matrix3x3::initialize(T_str_to_case);
    T_str_to_case[0][0] = -1.0;
    T_str_to_case[1][2] = T_str_to_case[2][1] = 1.0;
    return;
  }
  // Otherwise, the top row of the struc->case frame is the force_hat vector:
  // second row is cross-product of force_hat with struc-x-axis
  double scratch_y[3];
  scratch_y[0] = 0.0;
  scratch_y[1] = force_hat[2];
  scratch_y[2] = -force_hat[1];
  jeod::Vector3::normalize(scratch_y);
  // third row is completion - i.e. cross-product of force_hat with second-row.
  double scratch_z[3];
  jeod::Vector3::cross(force_hat, scratch_y, scratch_z);
  jeod::Vector3::normalize(scratch_z);
  // Assign:
  for (unsigned int ii = 0; ii < 3; ++ii) {
    T_str_to_case[0][ii] = force_hat[ii];
    T_str_to_case[1][ii] = scratch_y[ii];
    T_str_to_case[2][ii] = scratch_z[ii];
  }
}

/*****************************************************************************
apply_direction_error
Purpose:(applies the direction errors to generate a new force-direction)
*****************************************************************************/
void
RcsJet::apply_direction_error()
{
  if (direction_error == Vector) {
    jeod::Vector3::sum( force_hat,
                        force_hat_err,
                        force_hat_with_err); // not a unit vector!
  }
  else {
    // The new direction vector is cone-angle away from case-x-axis, in a
    // direction towards the y-axis after the y-z plane has been rotated by
    // azimuth-angle.
    double force_case[3];
    double sin_cone = std::sin(cone_angle_err);
    force_case[0] = std::cos(cone_angle_err);
    force_case[1] = sin_cone * std::cos(azimuth_angle_err);
    force_case[2] = sin_cone * std::sin(azimuth_angle_err);
    if (force_hat_changed) {
      generate_force_direction_matrix();
      force_hat_changed = false;
    }
    jeod::Vector3::transform_transpose( T_str_to_case,
                                        force_case,
                                        force_hat_with_err); // should be unit-vector
                                                             // but let's be safe
  }
  jeod::Vector3::normalize(force_hat_with_err);
}

/*****************************************************************************
apply_direction_dispersion
Purpose:(applies the run-constant direction dispersion to generate a new
         force-direction)
*****************************************************************************/
void
RcsJet::apply_direction_dispersion()
{
  // The new direction vector is cone-angle away from case-x-axis, in a
  // direction towards the y-axis after the y-z plane has been rotated by
  // azimuth-angle.
  double force_case[3];
  double sin_cone = std::sin(cone_angle_disp);
  force_case[0] = std::cos(cone_angle_disp);
  force_case[1] = sin_cone * std::cos(azimuth_angle_disp);
  force_case[2] = sin_cone * std::sin(azimuth_angle_disp);
  generate_force_direction_matrix();
  jeod::Vector3::transform_transpose( T_str_to_case,
                                      force_case,
                                      force_hat); // should be unit-vector
                                                  // but let's be safe
  jeod::Vector3::normalize(force_hat);
}

/*****************************************************************************
switch_status
Purpose:()
*****************************************************************************/
void
RcsJet::switch_status
  (RcsJetStatus new_status)
{
  status = new_status;

  if (status == Status_Off) {
    // Account for lost propellant when jet is turned off
    for (unsigned int ii=0; ii< component_consumption.size(); ++ii) {
      component_consumption.at(ii) += system.get_prop_loss_off(ii);
    }
  }

  else if (status == Status_On) {
    // Account for lost propellant when jet turned on
    for (unsigned int ii=0; ii< component_consumption.size(); ++ii) {
      component_consumption.at(ii) += system.get_prop_loss_on(ii);
    }
  }
}

/*****************************************************************************
scale_self_impingement
Purpose:(scales the user-specified base-impingement-force and -torque by
         an externally defined scale factor)
*****************************************************************************/
void
RcsJet::scale_self_impingement()
{
  jeod::Vector3::scale( base_impingement_force,
                        (scaled_force / force_cl),
                        scaled_impingement_force);
  jeod::Vector3::scale( base_impingement_torque,
                        (scaled_force / force_cl),
                        scaled_impingement_torque);
}

/*****************************************************************************
set_force_direction
Purpose:(Resets force_hat and renormalizes)
*****************************************************************************/
void
RcsJet::set_force_direction(double force_dir[3])
{
  if (force_dir == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"NULL argument.\n",
      "The force direction input was a NULL argument. Exiting without "
      "replacement.\n");
    return;
  }
  // Only compute if force_hat has changed.
  // Must be a perfect match to avoid computing.
  if ((MathUtils::is_near_equal(force_dir[0], force_hat[0])) &&
      (MathUtils::is_near_equal(force_dir[1], force_hat[1])) &&
      (MathUtils::is_near_equal(force_dir[2], force_hat[2]))) {
    return;
  }
  // Create the new force direction
  jeod::Vector3::copy(force_dir, force_hat);
  force_hat_changed = true; // for direction-matrix when applying errors.
  // if applying direction dispersions onto this new direction, apply them
  // now.  Otherwise, just make sure the new force-direction has been
  // normalized before continuing.
  if (direction_dispersion) {
    apply_direction_dispersion();
  }
  else {
    jeod::Vector3::normalize(force_hat);
  }
}

/*****************************************************************************
set_force_direction
Purpose:(Alternative implementation using 3 values instead of a
         preconstructed 3-array)
*****************************************************************************/
void
RcsJet::set_force_direction(
    double force_dir_x,
    double force_dir_y,
    double force_dir_z)
{
  double scratch[3];
  scratch[0] = force_dir_x;
  scratch[1] = force_dir_y;
  scratch[2] = force_dir_z;
  set_force_direction(scratch);
}
