/************************** TRICK HEADER ***************************************
PURPOSE:
  (Provide the functionality for the  Aero capabilities when the
   lookup table option is used.)

REFERENCE:
  (The CML aero models, written by Jeremy Rea, Jon Berndt, Sara McNamara,
   Sara Blatz, et, al.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares cleanup) (Adaptation from C-code))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include <cmath>   // sin, cos
#include <cstring> // NULL
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "../include/aero_executive_table.hh"

/*******************************************************************************
Constructors
*******************************************************************************/
AeroExecutiveTable::AeroExecutiveTable(AeroInterfaceOutput & output_ref,
                                       AeroEnvironment & environment_in,
                                       const bool   & disable_aero_moments_in,
                                       const bool   & disable_aero_damping_in,
                                       const double (&T_struc_to_body_in)[3][3])
  :
  AeroExecutiveBase( output_ref),
  T_struc_to_body( T_struc_to_body_in),
  environment( environment_in),
  disable_aero_moments( disable_aero_moments_in),
  disable_aero_damping( disable_aero_damping_in),

  T_body_to_aero_frame{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  cos_beta(1.0),
  sin_beta(0.0),
  cos_alpha(1.0),
  sin_alpha(0.0),
  cos_attack(1.0),
  sin_attack(0.0),
  cos_roll(1.0),
  sin_roll(0.0),
  moment_arm{0.0, 0.0, 0.0},

  T_struc_to_aero_frame{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
  data_table_type( AeroTableSetBase::Unspecified),
  uncertainties_expressed_as_percent(false),
  uncertainties_are_variable(false),
  aero_damping_in_table(false),
  aero_damping_on_diag_in_table( AeroTableSetBase::NotInTable),
  aero_damping_off_diag_in_table( AeroTableSetBase::NotInTable),

  current_table(NULL),

  threshold_min_free_stream_vel_mag(1.0), // Arbitrary threshold.

  dispersion_active(false),
  bias(),
  uncertainty(),
  random(),
  load_all_tables_at_init(false),
  l_over_v_scale( Lref_over_Vmag),
  mrc_position(),
  coefficients()
{}


/*******************************************************************************
change_table
Purpose: (Switch the selected table.)
*******************************************************************************/
void
AeroExecutiveTable::change_table( unsigned int new_ix)
{
  if (new_ix < data_tables_vector.size()) {
    if (current_table == data_tables_vector[new_ix]) {
      CMLMessage::inform(
        __FILE__,__LINE__,"Redundant request.\n",
        "Request to change table to the one at index ", new_ix, " (named ", current_table->name, "),\n"
        "but already using that table. No action taken.\n");
      return; // already on that table.
    }

    configure_new_table(data_tables_vector[new_ix]);

  }
  // if current_table == NULL, the model has not yet been initialized and will
  // fail at initialization if this is not corrected.
  else if (current_table == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Table request.\n",
      "Request to switch to table at index ", new_ix, ", but there are only ", data_tables_vector.size(), " tables "
      "available.\nRequest failed. No data table selected.\n");
  }
  else {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Table request.\n",
      "Request to switch to table at index ", new_ix, ", but there are only ", data_tables_vector.size(), " tables "
      "available.\nRequest failed. Continuing with existing table (", current_table->name, ").\n");
  }
  return;
}
/******************************************************************************/
void
AeroExecutiveTable::change_table( std::string new_name)
{
  // Check trivial case - change commanded to current table.
  if (current_table != NULL) {
    if (current_table->name.compare(new_name) == 0) {
      CMLMessage::inform(
        __FILE__,__LINE__,"Redundant request\n",
        "Request to change table to ", new_name, ", but already using that table.\n"
        "No action taken.\n");
      return; // already on that table.
    }
  }
  for (std::vector<AeroTableSetBase *>::iterator it = data_tables_vector.begin();
                                                 it != data_tables_vector.end();
                                                 ++it) {
    if ( (*it)->name.compare(new_name) == 0) {
      configure_new_table(*it);
      return;
    }
  }
  // if current_table == NULL, the model has not yet been initialized and will
  // fail at initialization if this is not corrected.
  if (current_table == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Table request.\n",
      "Request to switch to table named ", new_name, ", but none was found.\n"
      "Request failed. No table specified.\n");
  }
  else {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid Table request\n",
      "Request to switch to table named ", new_name, ", but none was found.\n"
      "Request failed. Will continue to use data table ", current_table->name, ".\n");
  }
}
//******************************************************************************
void
AeroExecutiveTable::change_table( AeroTableSetBase & new_table)
{
  // Just use the specified table whether it has been recognized or not.
  if (current_table == &new_table) {
    CMLMessage::inform(
      __FILE__,__LINE__,"Redundant request.\n",
      "Request to change table to ", new_table.name, ", but already using that table.\n"
      "No action taken.\n");
  }
  else {
    configure_new_table(&new_table);
  }
}

/*******************************************************************************
add_table
Purpose: (Adds a table to the available tables collection.)
*******************************************************************************/
void
AeroExecutiveTable::add_table( AeroTableSetBase * table)
{
  if (table == NULL) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Invalid address.\n",
      "The table to be added is NULL.\n"
      "Aborting addition\n");
    return;
  }

  for (std::vector<AeroTableSetBase *>::iterator it = data_tables_vector.begin();
                                                 it != data_tables_vector.end();
                                                 ++it) {
    // Check the addresses for duplicates.
    if ((*it) == table) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Duplication of Aero table.\n",
        "An AeroTableSetBase with this address has previously been added.\n"
        "Name of instance is ", table->name, ".\n"
        "Check configuration for possibility of duplicate additions.\n"
        "Continuing with the addition of this table.\n");
    }
    else if ((*it)->name.compare( table->name) == 0) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Duplication of Aero table.\n",
        "An AeroTableSetBase with this name (", (*it)->name, ") has previously been added.\n"
        "Caution should be exercised when switching between tables based\n"
        "on table-name which is now ambiguous.\n");
    }
  }

  data_tables_vector.push_back(table);
}

/*******************************************************************************
initialize
Purpose:(Verifies that all pointers etc are set.)
*******************************************************************************/
void
AeroExecutiveTable::initialize()
{
  // enabled and initialized come from AeroExec from SubscriptionBase
  if (initialized) {
    CMLMessage::inform(
      __FILE__,__LINE__, "Sequence configuration error.\n",
      "AeroExecutiveTable::initialize() was called after the instance was "
      "initialized.\nCheck your configuration.\n"
      "Ignoring duplicate request to initialize\n");
    return;
  }

  if (current_table == NULL) {
    if (data_tables_vector.empty()) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid Table initialization.\n",
        "No data tables have been loaded to provide data-lookup capabilities.\n"
        "This table-based Aero capability cannot be activated.\n");
      return; // Do not continue to initialize
    }
    else if (data_tables_vector[0] == NULL) {
      // UNREACHABLE CODE: but best to be safe.
      // data_tables_vector is protected, the only path to add a new table is
      // via add_table, which checks for NULL before pushing the new entry
      // onto the vector.
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid Table initialization\n",
        "Error of unknown source.\n"
        "A NULL data table was somehow loaded for data-lookup capabilities.\n"
        "This table-based Aero capability cannot be activated.\n");
      return; // Do not continue to initialize
    }
    else {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid Table initialization\n",
        "No data table has been selected to provide data-lookup capabilities.\n"
        "Using the first-loaded data table (", data_tables_vector[0]->name, ") by default.\n");
      change_table(0);
    }
  }

  // If all tables are loaded at initialization, go ahead and initialize them
  // all. If, however, tables are loaded only as needed, then the data does
  // not yet exist. They will have to be initialized mid-sim.
  // NOTE loading and initializing mid-sim runs the risk of faulting mid-sim.
  //      loading and initializing at the front-end adds time to load and
  //      initialize redundant tables.
  //      It is safer and slower to load and initialize at the front-end.
  if (load_all_tables_at_init) {
    for (std::vector<AeroTableSetBase *>::iterator it = data_tables_vector.begin();
                                                   it != data_tables_vector.end();
                                                   ++it) {
      if (!(*it)->is_initialized()) {
        (*it)->initialize();
      }
    }
  }

  // Push up the inheritance hierarchy
  AeroExecutiveBase::initialize();
}

/*******************************************************************************
update
Purpose: (The top level function call.)
*******************************************************************************/
void
AeroExecutiveTable::update()
{
  if (!active) {
    return;
  }

  // Read the latest values from the table.
  // Note - current_table is checked for non-NULL in activate(), and cannot be
  // reassigned (see activate() for details of why).
  current_table->update();

  // Generate trig functions for multiple application in post-processing
  trig_functions();

  // If dispersions are on, generate the bias dispersions (bias)
  // and apply those to the coefficients outputs generated either through the
  // data-table lookup or the processing of the data-table lookups.
  if (dispersion_active) {
    disperse_data();
  }

  // Generate the coefficients that are not provided by the table:
  post_process_table_data();

  // compute the outputs
  aero_forces_moments();
}

/*******************************************************************************
activate
Purpose: (Activates the method.)
*******************************************************************************/
void
AeroExecutiveTable::activate()
{
  if (current_table == NULL) {
    // UNREACHABLE CODE, sanity check.
    // activate() is called from SubscriptionBase and only when a model is
    // initialized.  This model cannot be initialized if current_table = NULL.
    // current_model is protected, and settable via configure_new_table(...);
    // this method has a NULL check preceding the current_table assignment, so
    // current_table cannot be reassigned to NULL post-initialization.
    // Because this is such a low-impact method, it is prudent to include this
    // completely redundant check here to safeguard future modifications.
    CMLMessage::error(
      __FILE__,__LINE__,"Activation error.\n",
      "No table has been selected for the AeroExecutive table-lookup."
      "Cannot activate.\n");
    return;
  }
  active = true;
}

/*******************************************************************************
configure_new_table
Purpose: (Actions necessary when a table changes.)
*******************************************************************************/
void
AeroExecutiveTable::configure_new_table( AeroTableSetBase * new_table)
{
  if (new_table == NULL) {
    // UNREACHABLE CODE, but best to be safe.
    // This method is protected, and is only called using an argument pulled
    // directly from the data_tables_vector STL-vector.
    // data_tables_vector is protected, the only path to add a new entry to
    // this vector is via add_table, which checks for NULL before pushing
    // the new entry onto the vector.
    // So data_tables_vector cannot hold NULL, so configure_new_table cannot be
    // called with a NULL argument.
    CMLMessage::fail(
      __FILE__,__LINE__,"Invalid call; impossible scenario\n",
      "Attempt to configure a new table but the new_table argument is NULL.\n"
      "This should not be possible; configure_new_table is a protected method\n"
      "and the vector from which the argument is drawn has NULL protection on\n"
      "it. If this error is reached, something is behaving unexpectedly.\n"
      "Terminating");
      // Exit here!!
  }

  // Turn off the current table
  if (current_table != NULL) {
    current_table->unsubscribe();
  }
  // reset to new table and configure new table.
  current_table = new_table;
  current_table->subscribe();
  current_table->configure_table();

  // Copy base data in from new table:
  Lref = current_table->Lref;
  Aref = current_table->Aref;

  jeod::Vector3::copy( current_table->mrc_position, mrc_position);
  data_table_type = current_table->get_table_type();

  aero_damping_in_table = current_table->get_damping_in_table();
  aero_damping_on_diag_in_table = current_table->get_damping_on_diag_in_table();
  aero_damping_off_diag_in_table = current_table->get_damping_off_diag_in_table();

  uncertainties_expressed_as_percent =
                            current_table->uncertainties_expressed_as_percent;
  process_prelim_uncertainties();

  jeod::Matrix3x3::copy( current_table->T_struc_to_aero_frame,
                   T_struc_to_aero_frame);
  jeod::Matrix3x3::product_right_transpose( T_struc_to_aero_frame,
                                      T_struc_to_body,
                                      T_body_to_aero_frame);
}

/*******************************************************************************
trig_functions
Purpose: (Compute trigonometric values.)
*******************************************************************************/
void
AeroExecutiveTable::trig_functions()
{
  cos_alpha = std::cos(environment.get_angle_of_attack());
  sin_alpha = std::sin(environment.get_angle_of_attack());
  cos_beta = std::cos(environment.get_angle_of_sideslip());
  sin_beta = std::sin(environment.get_angle_of_sideslip());
  if ( data_table_type == AeroTableSetBase::SYM_LDm ||
       data_table_type == AeroTableSetBase::SYM_ANm ||
       dispersion_active) {
    cos_attack = std::cos(environment.get_total_angle_of_attack());
    sin_attack = std::sin(environment.get_total_angle_of_attack());
    cos_roll = std::cos(environment.get_phi_roll());
    sin_roll = std::sin(environment.get_phi_roll());
  }
}

/*******************************************************************************
post_process_table_data
Purpose: (Make sure the table data is in the correct frames.)
*******************************************************************************/
void
AeroExecutiveTable::post_process_table_data()
{
  switch (data_table_type) {
  case AeroTableSetBase::SYM_LDm:
    coefficients.CN_sym =  coefficients.CL_sym * cos_attack +
                           coefficients.CD     * sin_attack;
    coefficients.CX     =  coefficients.CL_sym * sin_attack -
                           coefficients.CD     * cos_attack;
    coefficients.CY     = -coefficients.CN_sym * sin_roll;
    coefficients.CZ     = -coefficients.CN_sym * cos_roll;
    coefficients.CA     = -coefficients.CX;
    coefficients.CN     = -coefficients.CZ;
    // coefficients.CD provided
    coefficients.CS     = -coefficients.CX * sin_beta * cos_alpha +
                           coefficients.CY * cos_beta -
                           coefficients.CZ * sin_beta * sin_alpha;
    coefficients.CL     =  coefficients.CX * sin_alpha -
                           coefficients.CZ * cos_alpha;
    coefficients.Cl_mrc =  0.0;
    coefficients.Cm_mrc =  coefficients.Cm_sym * cos_roll;
    coefficients.Cn_mrc = -coefficients.Cm_sym * sin_roll;
    break;
  case AeroTableSetBase::SYM_ANm:
    coefficients.CL_sym =  coefficients.CN_sym * cos_attack -
                           coefficients.CA     * sin_attack;
    coefficients.CX     = -coefficients.CA;
    coefficients.CY     = -coefficients.CN_sym * sin_roll;
    coefficients.CZ     = -coefficients.CN_sym * cos_roll;
    coefficients.CN     = -coefficients.CZ;
    coefficients.CD     = -coefficients.CX     * cos_beta * cos_alpha -
                           coefficients.CY     * sin_beta -
                           coefficients.CZ     * cos_beta * sin_alpha;
    coefficients.CS     = -coefficients.CX * sin_beta * cos_alpha +
                           coefficients.CY * cos_beta -
                           coefficients.CZ * sin_beta * sin_alpha;
    coefficients.CL     =  coefficients.CX * sin_alpha -
                           coefficients.CZ * cos_alpha;
    coefficients.Cl_mrc =  0.0;
    coefficients.Cm_mrc =  coefficients.Cm_sym * cos_roll;
    coefficients.Cn_mrc = -coefficients.Cm_sym * sin_roll;
    break;
  case AeroTableSetBase::XYZ:
  case AeroTableSetBase::AYN:
  case AeroTableSetBase::AYN_unc:
    if (data_table_type == AeroTableSetBase::XYZ) {
      coefficients.CA = -coefficients.CX;
      coefficients.CN = -coefficients.CZ;
    }
    else { // AYN
      coefficients.CX = -coefficients.CA;
      coefficients.CZ = -coefficients.CN;
    }
    coefficients.CD = -coefficients.CX * cos_beta * cos_alpha -
                       coefficients.CY * sin_beta -
                       coefficients.CZ * cos_beta * sin_alpha;
    coefficients.CS = -coefficients.CX * sin_beta * cos_alpha +
                       coefficients.CY * cos_beta -
                       coefficients.CZ * sin_beta * sin_alpha;
    coefficients.CL =  coefficients.CX * sin_alpha -
                       coefficients.CZ * cos_alpha;
    break;
  case AeroTableSetBase::DSL:
      coefficients.CX = -coefficients.CD * cos_beta * cos_alpha -
                         coefficients.CS * sin_beta * cos_alpha +
                         coefficients.CL * sin_alpha;
      coefficients.CY = -coefficients.CD * sin_beta +
                         coefficients.CS * cos_beta;
      coefficients.CZ = -coefficients.CD * cos_beta * sin_alpha -
                         coefficients.CS * sin_beta * sin_alpha -
                         coefficients.CL * cos_alpha;
      coefficients.CA = -coefficients.CX;
      coefficients.CN = -coefficients.CZ;
    break;
  default:
    // UNREACHABLE CODE, sanity check.
    // The switch cases are exhaustive
    CMLMessage::fail(
      __FILE__,__LINE__,"Illegal operation mode\n",
      "AeroTable data mode not found.\n");
  }

  //*****************************************************************
  //   Compute the moment arms (vector from the CG to the MRC,
  //   in the aero frame)
  //*****************************************************************
  jeod::Vector3::diff( mrc_position,
                 environment.get_cg_position(),
                 moment_arm); // in the structural frame

  // Transform to the aero-frame:
  jeod::Vector3::transform( T_struc_to_aero_frame,
                      moment_arm);

  //*****************************************************************
  //   Compute the moment coefficients about the current CG
  //   cross product _cg = _mrc + (r x F)/L
  //*****************************************************************
  coefficients.Cl_cg =   coefficients.Cl_mrc +
     (moment_arm[1] * coefficients.CZ - moment_arm[2] * coefficients.CY) / Lref;
  coefficients.Cm_cg =   coefficients.Cm_mrc +
     (moment_arm[2] * coefficients.CX - moment_arm[0] * coefficients.CZ) / Lref;
  coefficients.Cn_cg =   coefficients.Cn_mrc +
     (moment_arm[0] * coefficients.CY - moment_arm[1] * coefficients.CX) / Lref;
}

/*******************************************************************************
aero_forces_moments
Purpose: (Computes forces and moments in the structural frame
          The coefficients: CX, CY, CZ, C*_cg, and C*_mrc are aligned with
          the aerodynamic frame, so their resulting values need transforming
          to struc.)
*******************************************************************************/
void
AeroExecutiveTable::aero_forces_moments()
{

  //*****************************************************************
  //   Compute the aerodynamic forces along the body axes
  //*****************************************************************
  double pA =  environment.get_dynamic_pressure() * Aref;
  output.force[0] =  coefficients.CX * pA;
  output.force[1] =  coefficients.CY * pA;
  output.force[2] =  coefficients.CZ * pA;
  jeod::Vector3::transform_transpose( T_struc_to_aero_frame,
                                output.force);
  //*****************************************************************
  // Compute the aerodynamic drag, side, and lift forces
  // NOTE - these remain in the aero-frame.
  //*****************************************************************
  output.drag_force = coefficients.CD * pA;
  output.side_force = coefficients.CS * pA;
  output.lift_force = coefficients.CL * pA;
  if (std::abs(coefficients.CD) < output.epsilon_CD_for_LoD) {
    output.LoD = 0.0;
  } else {
    output.LoD = coefficients.CL / coefficients.CD;
  }


  //*****************************************************************
  // Compute the aerodynamic moments along the body axes
  //*****************************************************************
  if (disable_aero_moments) {
    output.zero_torque();
    return;
  }
  // else:

  // Include the effects of rotational aerodynamic damping
  if ( aero_damping_in_table  && !disable_aero_damping) {
    double fsv_mag = environment.get_free_stream_vel_mag();
    if (fsv_mag > threshold_min_free_stream_vel_mag) {
      //   If fsv_mag is small, L_over_V is large, the coefficients get large
      //   and thus the torque gets large. Dimensionally, this is correct, and
      //   conceptually seems reasonable at reasonable speeds: this is the torque
      //   resulting fom the body rotating in the free stream, not the torque
      //   from the linear motion through the freestream.
      //   Added a safety threshold, set at construction time to prevent this
      //   from blowing up when unexpected winds produce small momentary
      //   free-stream velocities.
      double L_over_V = Lref / (l_over_v_scale * fsv_mag);
      double body_rate_aero_frm[3];
      jeod::Vector3::transform( T_body_to_aero_frame,
                          environment.get_true_body_rates(),
                          body_rate_aero_frm);

      coefficients.Cl_cg += ( body_rate_aero_frm[0] * coefficients.dCl_dp +
                              body_rate_aero_frm[1] * coefficients.dCl_dq +
                              body_rate_aero_frm[2] * coefficients.dCl_dr) *
                              L_over_V;
      coefficients.Cm_cg += ( body_rate_aero_frm[0] * coefficients.dCm_dp +
                              body_rate_aero_frm[1] * coefficients.dCm_dq +
                              body_rate_aero_frm[2] * coefficients.dCm_dr) *
                              L_over_V;
      coefficients.Cn_cg += ( body_rate_aero_frm[0] * coefficients.dCn_dp +
                              body_rate_aero_frm[1] * coefficients.dCn_dq +
                              body_rate_aero_frm[2] * coefficients.dCn_dr) *
                              L_over_V;
    }
  }
  double pAL = pA *  Lref;
  /* About the CG */
  output.torque[0] =  coefficients.Cl_cg * pAL;
  output.torque[1] =  coefficients.Cm_cg * pAL;
  output.torque[2] =  coefficients.Cn_cg * pAL;
  jeod::Vector3::transform_transpose( T_struc_to_aero_frame,
                                output.torque);

  /* About the MRC */
  output.moment_mrc[0] =  coefficients.Cl_mrc * pAL;
  output.moment_mrc[1] =  coefficients.Cm_mrc * pAL;
  output.moment_mrc[2] =  coefficients.Cn_mrc * pAL;
  jeod::Vector3::transform_transpose( T_struc_to_aero_frame,
                                output.moment_mrc);
}