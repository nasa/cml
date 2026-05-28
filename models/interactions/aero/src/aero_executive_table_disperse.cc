/*******************************TRICK HEADER************************************
PURPOSE: ()

PROGRAMMERS:
  (((Gary Turner) (OSR) (date) (project) (comment))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include "../include/aero_executive_table.hh"

/*******************************************************************************
process_prelim_uncertainties
Purpose: (For the tables in which the uncertainties are specified and fixed,
          i.e. not an element of a table lookup.)
*******************************************************************************/
void
AeroExecutiveTable::process_prelim_uncertainties()
{
  // For each coefficient:
  // - The table has a nominal uncertainty values; this is copied to
  //     the local value.
  // - Random is a random number, usually in the range [-1,1] used for
  //     monte-carlo analysis. It is constant for the simulation, and applied
  //     equally for all tables.
  // - Uncertainty is initially populated with the table nominal value.
  //     If the uncertainty varies, (i.e. if the table provides lookup-values
  //     for uncertainties), it may be overridden later. It may represent a
  //     relative (percentage) value or an absolute value.
  // - Modified_uncertainty is the product of the uncertainty and the random
  //     number; it represents the random application at some point within
  //     the full range of the uncertainty.
  // - Bias is the value that actually gets added to the nominal coefficient
  //     value. It is computed as either:
  //     - random * uncertainty           (when uncertainty represents an
  //                                       absolute value)
  //     - random * uncertainty * nominal (when uncertainty represents a
  //                                       relative / percentage value)
  //     Its initial value is set here. In the case that uncertainty is NOT a
  //     part of the table-lookup, AND uncertainty represents an absolute
  //     value, the value computed here will be retained. Otherwise, it is
  //     subject to later modification as either (or both) uncertainty and
  //     nominal values change..

  // For data tables that include specification of uncertainties, set the
  // uncertainties_are_variable flag.  This will force a call to
  // modify_uncertainties on every pass.
  if (data_table_type == AeroTableSetBase::AYN_unc) {
    uncertainties_are_variable = true;
  }
  // For all other types, leave the flag at is default setting (false) or
  // user-specified override.
  // Note -- it would be possible to have set the
  // uncertainties_are_variable flag manually, in which case
  // modify_uncertainties() will be called at every cycle and does not
  // need to be called now.  But if the flag is still in its default state
  // of false, call modify_uncertainties now, once and for all time.
  if (!uncertainties_are_variable) {
    populate_uncertainties();
    modify_uncertainties();
  }

  // The damping coefficients work similarly -- if the table includes the
  // uncertainty values, execute the modifications on every cycle.
  // Otherwise, execute them now.
  if (aero_damping_in_table) {
    switch ( aero_damping_on_diag_in_table) {
    case  AeroTableSetBase::NotInTable:
      // Note -- in the case that aero-damping is flagged as being included
      // in the table but the on-diagonal components are set as being
      // "NotInTable", set those on-diagonal components to zero.
      coefficients.dCl_dp = 0.0;
      coefficients.dCm_dq = 0.0;
      coefficients.dCn_dr = 0.0;
      break;
    case AeroTableSetBase::CoeffsOnly:
      modify_damping_uncertainties_on_diagonal();
      break;
    case AeroTableSetBase::CoeffsUnc:
      // no-op, modify_damping_uncertainties_on_diagonal() called on each
      // cycle
      break;
    default:
      // UNREACHABLE CODE.  Switch cases are exhaustive.
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid AeroDamping Type specification.\n",
        "Unrecognized AeroDamping type.\n"
        "This switch statement should be exhaustive.\n");
      break;
    }

    switch (aero_damping_off_diag_in_table) {
    case  AeroTableSetBase::NotInTable:
      // Note -- in the case that aero-damping is flagged as being included
      // in the table but the off-diagonal components are set as being
      // "NotInTable", set those off-diagonal components to zero.
      coefficients.dCl_dq = 0.0;
      coefficients.dCl_dr = 0.0;
      coefficients.dCm_dp = 0.0;
      coefficients.dCm_dr = 0.0;
      coefficients.dCn_dp = 0.0;
      coefficients.dCn_dq = 0.0;
      break;
    case AeroTableSetBase::CoeffsOnly:
      modify_damping_uncertainties_off_diagonal();
      break;
    case AeroTableSetBase::CoeffsUnc:
      // no-op, modify_damping_uncertainties_off_diagonal() called on each
      // cycle
      break;
    default:
      // UNREACHABLE CODE.  Switch cases are exhaustive.
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid AeroDamping Type specification.\n",
        "Unrecognized AeroDamping type.\n"
        "This switch statement should be exhaustive.\n");
      break;
    }
  }
}

/*****************************************************************************
populate_uncertainties
Purpose:
  - This method is called only from process_prelim_uncertainties() when
    a table is first being established as the data source for the
    executive -- and then only if the uncertainties are not updated
    from the interpolation table.
  - The method pulls the default uncertainty values from the table,
    assigning them into the executive (i.e. this class).
    These values will then be modified by applying a random scaling in
    method modify_uncertainties()
  - Note that in the case that uncertainties are populated with each
    table lookup, then modify_uncertainties() gets called with each
    table lookup.  In this case, the table-lookup will directly populate
    the uncertainty structure in this class, so modify_uncertainties()
    operates on that structure.  This method populates that structure from
    the table default values, allowing modify_uncertainties() to do its
    thing.
*****************************************************************************/
void
AeroExecutiveTable::populate_uncertainties()
{
  switch (data_table_type) {
    case AeroTableSetBase::Unspecified:
      // UNREACHABLE CODE. Access to this (protected) method comes from
      // AeroExecTable::configure_table(), which has already run the
      // AeroTableSetBase::initialize() method on the table in question.
      // That method has a terminal fault on an unspecified data-type.
      // There is no path to change the data-type post-initialization.
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid data table type specification.\n",
        "Data table type has not been specified.\n"
        "This error should have already been caught.\n"
        "This message should never be seen.\n");
      break;
    case AeroTableSetBase::SYM_LDm:
      uncertainty.CD = current_table->uncertainty.CD;
      uncertainty.CL_sym = current_table->uncertainty.CL_sym;
      uncertainty.Cm_sym = current_table->uncertainty.Cm_sym;
      break;
    case AeroTableSetBase::SYM_ANm:
      uncertainty.CA = current_table->uncertainty.CA;
      uncertainty.CN_sym = current_table->uncertainty.CN_sym;
      uncertainty.Cm_sym = current_table->uncertainty.Cm_sym;
      break;
    case AeroTableSetBase::XYZ:
      uncertainty.CX = current_table->uncertainty.CX;
      uncertainty.CY = current_table->uncertainty.CY;
      uncertainty.CZ = current_table->uncertainty.CZ;
      uncertainty.Cl = current_table->uncertainty.Cl;
      uncertainty.Cm = current_table->uncertainty.Cm;
      uncertainty.Cn = current_table->uncertainty.Cn;
      break;
    case AeroTableSetBase::AYN:
    case AeroTableSetBase::AYN_unc:
      uncertainty.CA = current_table->uncertainty.CA;
      uncertainty.CY = current_table->uncertainty.CY;
      uncertainty.CN = current_table->uncertainty.CN;
      uncertainty.Cl = current_table->uncertainty.Cl;
      uncertainty.Cm = current_table->uncertainty.Cm;
      uncertainty.Cn = current_table->uncertainty.Cn;
      break;
    case AeroTableSetBase::DSL:
      uncertainty.CD = current_table->uncertainty.CD;
      uncertainty.CS = current_table->uncertainty.CS;
      uncertainty.CL = current_table->uncertainty.CL;
      uncertainty.Cl = current_table->uncertainty.Cl;
      uncertainty.Cm = current_table->uncertainty.Cm;
      uncertainty.Cn = current_table->uncertainty.Cn;
      break;
    default:
      // UNREACHABLE CODE.  Switch cases are exhaustive.
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid data table type specification.\n",
        "Unrecognized data table type.\n"
        "This switch statement should be exhaustive.\n"
        "This error should have already been caught.\n"
        "This message should never be seen.\n");
      break;
  }
}

/*******************************************************************************
disperse_data
Purpose: (Disperse the values in the AeroCurrent structure.)
NOTE - The values uncertainty.* are either preset or have just been looked
       up in the table
*******************************************************************************/
void
AeroExecutiveTable::disperse_data()
{
  // Applying a bias is a 3-step process:
  // 1. if the uncertainties are variable, recompute the
  //    modified-uncertainties (product of uncertainty and random value)
  // 2. If the uncertainties are expressed as a scaling factor, scale the
  //    most recent value of each coefficient with its scaling factor
  //    to generate an additive bias.
  // 3. Add the bias to the most recent value of each coefficient.

  if (uncertainties_are_variable) {
    modify_uncertainties();
  }

  if (uncertainties_expressed_as_percent ) {
    scale_bias();
  }

  increment_coeffs_with_bias();


  // Do the same for the damping coefficients if damping coefficients are
  // present.
  if (aero_damping_in_table) {
    if (aero_damping_on_diag_in_table == AeroTableSetBase::CoeffsUnc) {
      modify_damping_uncertainties_on_diagonal();
    }
    if (aero_damping_off_diag_in_table == AeroTableSetBase::CoeffsUnc) {
      modify_damping_uncertainties_off_diagonal();
    }

    if (uncertainties_expressed_as_percent ) {
      scale_damping_bias();
    }

    increment_damping_coeffs_with_bias();
  }
}






/*****************************************************************************
modify_uncertainties
Purpose: accesses the uncertainty and the random scale-factor for each
         coefficient in the specified coefficient-set.
         - The uncertainty is typically user-specified but may also be
           table-driven
         - the random scale-factor is typically generated in a MonteCarlo
           configuration and is typically held constant for the duration,
           even if the uncertainty value changes.

         Multiply these two values to generate a modified-uncertainty.

         In each case, assign this value also the the "bias" for each
         coefficient. The bias is the value that gets added to the nominal
         value to generate the dispersed value.
         - In the case that the dispersion is configured to be "absolute", the
         bias is equal to the modified-uncertainty.
         - In the case that the dispersion is configured to be "relative", the
         bias is (1+modified_uncertainty)*nominal; in this case the bias will be
         recomputed in scale_bias().
*****************************************************************************/
void
AeroExecutiveTable::modify_uncertainties()
{
  switch (data_table_type) {
    case AeroTableSetBase::Unspecified:
      // UNREACHABLE CODE. Access to this (protected) method comes from
      // AeroExecTable::configure_table(), which has already run the
      // AeroTableSetBase::initialize() method on the table in question.
      // That method has a terminal fault on an unspecified data-type.
      // There is no path to change the data-type post-initialization.
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid data table type specification.\n",
        "Data table type has not been specified.\n"
        "This error should have already been caught.\n"
        "This message should never be seen.\n");
      break;
    case AeroTableSetBase::SYM_LDm:
      bias.CD =
      modified_uncertainty.CD = uncertainty.CD * random.CD;
      bias.CL_sym =
      modified_uncertainty.CL_sym = uncertainty.CL_sym * random.CL_sym;
      bias.Cm_sym =
      modified_uncertainty.Cm_sym = uncertainty.Cm_sym * random.Cm_sym;
      break;
    case AeroTableSetBase::SYM_ANm:
      bias.CA =
      modified_uncertainty.CA = uncertainty.CA * random.CA;
      bias.CN_sym =
      modified_uncertainty.CN_sym = uncertainty.CN_sym * random.CN_sym;
      bias.Cm_sym =
      modified_uncertainty.Cm_sym = uncertainty.Cm_sym * random.Cm_sym;
      break;
    case AeroTableSetBase::XYZ:
      bias.CX =
      modified_uncertainty.CX = uncertainty.CX * random.CX;
      bias.CY =
      modified_uncertainty.CY = uncertainty.CY * random.CY;
      bias.CZ =
      modified_uncertainty.CZ = uncertainty.CZ * random.CZ;
      bias.Cl =
      modified_uncertainty.Cl = uncertainty.Cl * random.Cl;
      bias.Cm =
      modified_uncertainty.Cm = uncertainty.Cm * random.Cm;
      bias.Cn =
      modified_uncertainty.Cn = uncertainty.Cn * random.Cn;
      break;
    case AeroTableSetBase::AYN:
    case AeroTableSetBase::AYN_unc:
      bias.CA =
      modified_uncertainty.CA = uncertainty.CA * random.CA;
      bias.CY =
      modified_uncertainty.CY = uncertainty.CY * random.CY;
      bias.CN =
      modified_uncertainty.CN = uncertainty.CN * random.CN;
      bias.Cl =
      modified_uncertainty.Cl = uncertainty.Cl * random.Cl;
      bias.Cm =
      modified_uncertainty.Cm = uncertainty.Cm * random.Cm;
      bias.Cn =
      modified_uncertainty.Cn = uncertainty.Cn * random.Cn;
      break;
    case AeroTableSetBase::DSL:
      bias.CD =
      modified_uncertainty.CD = uncertainty.CD * random.CD;
      bias.CS =
      modified_uncertainty.CS = uncertainty.CS * random.CS;
      bias.CL =
      modified_uncertainty.CL = uncertainty.CL * random.CL;
      bias.Cl =
      modified_uncertainty.Cl = uncertainty.Cl * random.Cl;
      bias.Cm =
      modified_uncertainty.Cm = uncertainty.Cm * random.Cm;
      bias.Cn =
      modified_uncertainty.Cn = uncertainty.Cn * random.Cn;
      break;
    default:
      // UNREACHABLE CODE.  Switch cases are exhaustive.
      CMLMessage::fail(
        __FILE__,__LINE__,"Invalid data table type specification.\n",
        "Unrecognized data table type.\n"
        "This switch statement should be exhaustive.\n"
        "This error should have already been caught.\n"
        "This message should never be seen.\n");
      break;
  }
}

/*****************************************************************************
modify_damping_uncertainties_on_diagonal
Purpose: Applies random modification to uncertainties for the diagonal
         damping coefficients.
Note - This protected method will only be called if aero-damping is flagged
       as being in the table, and specifically the on-diagonal components of
       the damping coefficients.
*****************************************************************************/
void
AeroExecutiveTable::modify_damping_uncertainties_on_diagonal()
{
  uncertainty.dCl_dp = current_table->uncertainty.dCl_dp;
  uncertainty.dCm_dq = current_table->uncertainty.dCm_dq;
  uncertainty.dCn_dr = current_table->uncertainty.dCn_dr;

  bias.dCl_dp =
  modified_uncertainty.dCl_dp = uncertainty.dCl_dp * random.dCl_dp;
  bias.dCm_dq =
  modified_uncertainty.dCm_dq = uncertainty.dCm_dq * random.dCm_dq;
  bias.dCn_dr =
  modified_uncertainty.dCn_dr = uncertainty.dCn_dr * random.dCn_dr;
}

/*****************************************************************************
modify_damping_uncertainties_off_diagonal
Purpose: Applies random modification to uncertainties for the off-diagonal
         damping coefficients.
Note - This protected method will only be called if aero-damping is flagged
       as being in the table, and specifically the off-diagonal components of
       the damping coefficients.
*****************************************************************************/
void
AeroExecutiveTable::modify_damping_uncertainties_off_diagonal()
{
  uncertainty.dCl_dq = current_table->uncertainty.dCl_dq;
  uncertainty.dCl_dr = current_table->uncertainty.dCl_dr;
  uncertainty.dCm_dp = current_table->uncertainty.dCm_dp;
  uncertainty.dCm_dr = current_table->uncertainty.dCm_dr;
  uncertainty.dCn_dp = current_table->uncertainty.dCn_dp;
  uncertainty.dCn_dq = current_table->uncertainty.dCn_dq;

  bias.dCl_dq =
  modified_uncertainty.dCl_dq = uncertainty.dCl_dq * random.dCl_dq;
  bias.dCl_dr =
  modified_uncertainty.dCl_dr = uncertainty.dCl_dr * random.dCl_dr;
  bias.dCm_dp =
  modified_uncertainty.dCm_dp = uncertainty.dCm_dp * random.dCm_dp;
  bias.dCm_dr =
  modified_uncertainty.dCm_dr = uncertainty.dCm_dr * random.dCm_dr;
  bias.dCn_dp =
  modified_uncertainty.dCn_dp = uncertainty.dCn_dp * random.dCn_dp;
  bias.dCn_dq =
  modified_uncertainty.dCn_dq = uncertainty.dCn_dq * random.dCn_dq;
}


/*******************************************************************************
recompute_bias
Purpose: (Recomputes the bias values in the case that bias is not constant.)
*******************************************************************************/
void
AeroExecutiveTable::scale_bias()
{
  // Called only if uncertainties are relative/percentages, so bias is a
  // function of the nominal coefficient values. This method will overwrite
  // the values set in modify_uncertainties().
  //
  // Note that coefficients get reset every cycle by a table-lookup, so the
  // subsequent increment "coeff += bias" does not accumulate.
  switch (data_table_type) {
    case AeroTableSetBase::SYM_LDm:
      //******************************************************************
      //    Symmetric vehicle:  Input lift, drag, and moment coefficients.
      //    Disperse the following: CD, CL_sym, Cm_sym
      //******************************************************************
      bias.CD      = modified_uncertainty.CD * coefficients.CD;
      bias.CL_sym  = modified_uncertainty.CL_sym * coefficients.CL_sym;
      bias.Cm_sym  = modified_uncertainty.Cm_sym * coefficients.Cm_sym;
      break;

    case AeroTableSetBase::SYM_ANm:
      //******************************************************************
      //    Symmetric vehicle:  Input axial force, normal force,
      //                        and moment coefficients.
      //    Disperse the following: CA, CN_sym, Cm_sym
      //******************************************************************
      bias.CA      = modified_uncertainty.CA     * coefficients.CA;
      bias.CN_sym  = modified_uncertainty.CN_sym * coefficients.CN_sym;
      bias.Cm_sym  = modified_uncertainty.Cm_sym * coefficients.Cm_sym;
      break;

    case AeroTableSetBase::XYZ:
      //******************************************************************
      //    Non-Symmetric vehicle:  Input XYZ-body force and
      //                            moment coefficients.
      //    Disperse the following: CX, CY, CZ, Cl_mrc, Cm_mrc, Cn_mrc
      //******************************************************************
      bias.CX  = modified_uncertainty.CX * coefficients.CX;
      bias.CY  = modified_uncertainty.CY * coefficients.CY;
      bias.CZ  = modified_uncertainty.CZ * coefficients.CZ;
      bias.Cl  = modified_uncertainty.Cl * coefficients.Cl_mrc;
      bias.Cm  = modified_uncertainty.Cm * coefficients.Cm_mrc;
      bias.Cn  = modified_uncertainty.Cn * coefficients.Cn_mrc;
      break;

    case AeroTableSetBase::AYN:
    case AeroTableSetBase::AYN_unc:
      //******************************************************************
      //    Non-Symmetric vehicle:  Input axial, Y-body, and normal force
      //                            and body moment coefficients including
      //                            slope term.
      //    Disperse the following: CA, CY, CN, Cl, Cm, Cn
      //******************************************************************
      //  For the AYN_unc case, uncertainties have likely been updated by
      //  the table update and those raw values re-modified ini
      // modify_uncertainties.
      bias.CA  = modified_uncertainty.CA * coefficients.CA;
      bias.CN  = modified_uncertainty.CN * coefficients.CN;
      bias.CY  = modified_uncertainty.CY * coefficients.CY;
      bias.Cl  = modified_uncertainty.Cl * coefficients.Cl_mrc;
      bias.Cm  = modified_uncertainty.Cm * coefficients.Cm_mrc;
      bias.Cn  = modified_uncertainty.Cn * coefficients.Cn_mrc;
      break;

    case AeroTableSetBase::DSL:
      //******************************************************************
      //    Non-Symmetric vehicle:  Input drag, side, and lift force
      //                            and body moment coefficients
      //    Disperse the following: CD, CS, CL, Cl, Cm, Cn
      //******************************************************************
      bias.CD  = modified_uncertainty.CD * coefficients.CD;
      bias.CS  = modified_uncertainty.CS * coefficients.CS;
      bias.CL  = modified_uncertainty.CL * coefficients.CL;
      bias.Cl  = modified_uncertainty.Cl * coefficients.Cl_mrc;
      bias.Cm  = modified_uncertainty.Cm * coefficients.Cm_mrc;
      bias.Cn  = modified_uncertainty.Cn * coefficients.Cn_mrc;
      break;

    case AeroTableSetBase::Unspecified:
      // UNREACHABLE CODE. Cannot get here with "Unspecified" as the type.
      // Would already have failed out during initialization, and data-type
      // cannot be changed post-initialization.
      CMLMessage::fail(
        __FILE__,__LINE__,"Illegal operation mode\n",
        "Cannot execute a table identified with Unspecified data type.\n");
        break;
    default:
      // UNREACHABLE CODE.  List is exhaustive.
      CMLMessage::fail(
        __FILE__,__LINE__,"Illegal operation mode\n",
        "AeroTable data mode not found.\n");
  }
}

/*****************************************************************************
scale_damping_bias
Purpose: Scales the damping-coefficients to generate the
         damping-coefficient-biases
*****************************************************************************/
void
AeroExecutiveTable::scale_damping_bias()
{
  if (aero_damping_on_diag_in_table != AeroTableSetBase::NotInTable) {
    bias.dCl_dp = modified_uncertainty.dCl_dp * coefficients.dCl_dp;
    bias.dCm_dq = modified_uncertainty.dCm_dq * coefficients.dCm_dq;
    bias.dCn_dr = modified_uncertainty.dCn_dr * coefficients.dCn_dr;
  }
  // Otherwise, no action, the bias will not be applied.

  if (aero_damping_off_diag_in_table != AeroTableSetBase::NotInTable) {
    bias.dCl_dq = modified_uncertainty.dCl_dq * coefficients.dCl_dq;
    bias.dCl_dr = modified_uncertainty.dCl_dr * coefficients.dCl_dr;
    bias.dCm_dp = modified_uncertainty.dCm_dp * coefficients.dCm_dp;
    bias.dCm_dr = modified_uncertainty.dCm_dr * coefficients.dCm_dr;
    bias.dCn_dp = modified_uncertainty.dCn_dp * coefficients.dCn_dp;
    bias.dCn_dq = modified_uncertainty.dCn_dq * coefficients.dCn_dq;
  }
}



/*****************************************************************************
increment_coeffs_with_bias
Purpose: Increment the raw coefficient with the generated bias values.
*****************************************************************************/
void
AeroExecutiveTable::increment_coeffs_with_bias()
{
  switch (data_table_type) {
    case AeroTableSetBase::SYM_LDm:
      //******************************************************************
      //    Symmetric vehicle:  Input lift, drag, and moment coefficients.
      //    Disperse the following: CD, CL_sym, Cm_sym
      //******************************************************************
      coefficients.CD += bias.CD;
      coefficients.CL_sym += bias.CL_sym;
      coefficients.Cm_sym += bias.Cm_sym;
      break;

    case AeroTableSetBase::SYM_ANm:
      //******************************************************************
      //    Symmetric vehicle:  Input axial force, normal force,
      //                        and moment coefficients.
      //    Disperse the following: CA, CN_sym, Cm_sym
      //******************************************************************
      coefficients.CA += bias.CA;
      coefficients.CN_sym += bias.CN_sym;
      coefficients.Cm_sym += bias.Cm_sym;
      break;

    case AeroTableSetBase::XYZ:
      //******************************************************************
      //    Non-Symmetric vehicle:  Input XYZ-body force and
      //                            moment coefficients.
      //    Disperse the following: CX, CY, CZ, Cl_mrc, Cm_mrc, Cn_mrc
      //******************************************************************
      coefficients.CX += bias.CX;
      coefficients.CY += bias.CY;
      coefficients.CZ += bias.CZ;
      coefficients.Cl_mrc += bias.Cl;
      coefficients.Cm_mrc += bias.Cm;
      coefficients.Cn_mrc += bias.Cn;
      break;

    case AeroTableSetBase::AYN:
    case AeroTableSetBase::AYN_unc:
      //******************************************************************
      //    Non-Symmetric vehicle:  Input axial, Y-body, and normal force
      //                            and body moment coefficients including
      //                            slope term.
      //    Disperse the following: CA, CY, CN, Cl, Cm, Cn
      //******************************************************************
      //  For the AYN_unc case, uncertainties have likely been updated by
      //  the table update and those raw values re-modified ini
      // modify_uncertainties.
      coefficients.CA += bias.CA;
      coefficients.CN += bias.CN;
      coefficients.CY += bias.CY;
      coefficients.Cl_mrc += bias.Cl;
      coefficients.Cm_mrc += bias.Cm;
      coefficients.Cn_mrc += bias.Cn;
      break;

    case AeroTableSetBase::DSL:
      //******************************************************************
      //    Non-Symmetric vehicle:  Input drag, side, and lift force
      //                            and body moment coefficients
      //    Disperse the following: CD, CS, CL, Cl, Cm, Cn
      //******************************************************************
      coefficients.CD += bias.CD;
      coefficients.CS += bias.CS;
      coefficients.CL += bias.CL;
      coefficients.Cl_mrc += bias.Cl;
      coefficients.Cm_mrc += bias.Cm;
      coefficients.Cn_mrc += bias.Cn;
      break;

    case AeroTableSetBase::Unspecified:
      // UNREACHABLE CODE. Cannot get here with "Unspecified" as the type.
      // Would already have failed out during initialization, and data-type
      // cannot be changed post-initialization.
      CMLMessage::fail(
        __FILE__,__LINE__,"Illegal operation mode\n",
        "Cannot execute a table identified with Unspecified data type.\n");
        break;
    default:
      // UNREACHABLE CODE.  List is exhaustive.
      CMLMessage::fail(
        __FILE__,__LINE__,"Illegal operation mode\n",
        "AeroTable data mode not found.\n");
  }
}

/*****************************************************************************
increment_damping_coeffs_with_bias
Purpose: Increments the damping-coefficients with the
         damping-coefficient-biases
*****************************************************************************/
void
AeroExecutiveTable::increment_damping_coeffs_with_bias()
{
  if (aero_damping_on_diag_in_table != AeroTableSetBase::NotInTable) {
    coefficients.dCl_dp += bias.dCl_dp;
    coefficients.dCm_dq += bias.dCm_dq;
    coefficients.dCn_dr += bias.dCn_dr;
  }
  if (aero_damping_off_diag_in_table != AeroTableSetBase::NotInTable) {
    coefficients.dCl_dq += bias.dCl_dq;
    coefficients.dCl_dr += bias.dCl_dr;
    coefficients.dCm_dp += bias.dCm_dp;
    coefficients.dCm_dr += bias.dCm_dr;
    coefficients.dCn_dp += bias.dCn_dp;
    coefficients.dCn_dq += bias.dCn_dq;
  }
}