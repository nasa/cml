/*******************************TRICK HEADER************************************
PURPOSE: (Provides the methods for the AeroTableSet class)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (Antares) (initial version))
   ((Gary Turner) (OSR) (June 2016) (Antares) (Refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/utils/math/include/vector3.hh"
#include "jeod/models/utils/math/include/matrix3x3.hh"

#include "../include/aero_table_set_base.hh"

/*******************************************************************************
Constructor
*******************************************************************************/
AeroTableSetBase::AeroTableSetBase(
     const std::string & name_in,
     AeroCoefficientsTable & coefficients_out,
     AeroCoefficientsDisp  & uncertainties_out)
   :
   TableLookupSet(),

   coefficients( coefficients_out),
   uncertainty_ref( uncertainties_out),

   name( name_in),
   Aref(0.0),
   Lref(0.0),
   T_struc_to_aero_frame{{1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{0.0, 0.0, 1.0}},
   mrc_position{0.0, 0.0, 0.0},
   uncertainties_expressed_as_percent(false),
   uncertainty(),
   aero_damping_in_table(false),
   data_table_type( Unspecified),
   aero_damping_on_diag_in_table( NotInTable),
   aero_damping_off_diag_in_table( NotInTable),
   uncertainty_data_present(false),
   coef_data_present(false)
{}

/*******************************************************************************
initialize
*******************************************************************************/
void
AeroTableSetBase::initialize()
{
  if (initialized) {
    return;
  }

  TableLookupSet::initialize();
  if(!initialized) { // base-class initialization failed.
    return;
  }

  bool data_found = false;
  switch (data_table_type) {
  case SYM_LDm:
    data_found = (is_a_dependent_variable(coefficients.CD) &&
                  is_a_dependent_variable(coefficients.CL_sym) &&
                  is_a_dependent_variable(coefficients.Cm_sym));
    break;
  case SYM_ANm:
    data_found = (is_a_dependent_variable(coefficients.CA) &&
                  is_a_dependent_variable(coefficients.CN_sym) &&
                  is_a_dependent_variable(coefficients.Cm_sym));
    break;
  case XYZ:
    data_found = (is_a_dependent_variable(coefficients.CX) &&
                  is_a_dependent_variable(coefficients.CY) &&
                  is_a_dependent_variable(coefficients.CZ) &&
                  is_a_dependent_variable(coefficients.Cl_mrc) &&
                  is_a_dependent_variable(coefficients.Cm_mrc) &&
                  is_a_dependent_variable(coefficients.Cn_mrc));
    break;
  case AYN_unc:
    data_found = (is_a_dependent_variable(uncertainty_ref.CA) &&
                  is_a_dependent_variable(uncertainty_ref.CY) &&
                  is_a_dependent_variable(uncertainty_ref.CN) &&
                  is_a_dependent_variable(uncertainty_ref.Cl) &&
                  is_a_dependent_variable(uncertainty_ref.Cm) &&
                  is_a_dependent_variable(uncertainty_ref.Cn));
    if (!data_found) {
      break;
    }
    // else fall-through to check the coefficients as well
    [[fallthrough]];
  case AYN:
    data_found = (is_a_dependent_variable(coefficients.CA) &&
                  is_a_dependent_variable(coefficients.CY) &&
                  is_a_dependent_variable(coefficients.CN) &&
                  is_a_dependent_variable(coefficients.Cl_mrc) &&
                  is_a_dependent_variable(coefficients.Cm_mrc) &&
                  is_a_dependent_variable(coefficients.Cn_mrc));
    break;
  case DSL:
    data_found = (is_a_dependent_variable(coefficients.CD) &&
                  is_a_dependent_variable(coefficients.CS) &&
                  is_a_dependent_variable(coefficients.CL) &&
                  is_a_dependent_variable(coefficients.Cl_mrc) &&
                  is_a_dependent_variable(coefficients.Cm_mrc) &&
                  is_a_dependent_variable(coefficients.Cn_mrc));
    break;
  case Unspecified:
    CMLMessage::fail(
      __FILE__,__LINE__,"Illegal operation mode\n",
      "AeroTable data mode has not been specified.\n"
      "Specified data mode is still at the default 'Unspecified'.\n");
    break;
  default:
    // UNREACHABLE CODE; sanity check
    // Switch cases should be exhaustive.
    CMLMessage::fail(
      __FILE__,__LINE__,"Illegal operation mode\n",
      "AeroTable data mode not found.\n"
      "Specified data mode is ", data_table_type, "\n");
  }
  if (!data_found) {
    CMLMessage::fail(
      __FILE__,__LINE__,"Incomplete data table.\n",
      "The associated data tables do not contain the data necessary for the\n"
      "selected data mode.\nError in table-set ", name, " for data-mode ", data_table_type, ".\n");
  }
}

/*******************************************************************************
configure_table
Purpose: (Actions necessary when a table changes.)
*******************************************************************************/
void
AeroTableSetBase::configure_table()
{
  // If table has not yet been initialized, initialize it:
  if (!initialized) {
    initialize();
  }

  // Identify which aero-damping values are being populated by this table:
  query_aero_damping();
}

/*******************************************************************************
query_aero_damping
Purpose: (Identifies whether there are any aero-damping terms in the table.)
*******************************************************************************/
void
AeroTableSetBase::query_aero_damping()
{
  query_on_diag_aero_damping();
  query_off_diag_aero_damping();
  aero_damping_in_table = aero_damping_on_diag_in_table ||
                          aero_damping_off_diag_in_table;
  return;
}

/*******************************************************************************
query_on_diag_aero_damping
Purpose: (Identifies whether there are any principle aero-damping terms
          in the table.)
*******************************************************************************/
void
AeroTableSetBase::query_on_diag_aero_damping()
{
  coef_data_present = (is_a_dependent_variable(coefficients.dCl_dp) ||
                       is_a_dependent_variable(coefficients.dCm_dq) ||
                       is_a_dependent_variable(coefficients.dCn_dr));

  uncertainty_data_present = (
                       is_a_dependent_variable(uncertainty_ref.dCl_dp) ||
                       is_a_dependent_variable(uncertainty_ref.dCm_dq) ||
                       is_a_dependent_variable(uncertainty_ref.dCn_dr));

  aero_damping_on_diag_in_table = verify_aero_damping("on-diagonal");
}

/*******************************************************************************
query_aero_damping
Purpose: (Identifies whether there are any cross-axis aero-damping terms in the
          table.)
*******************************************************************************/
void
AeroTableSetBase::query_off_diag_aero_damping()
{
  coef_data_present = (is_a_dependent_variable(coefficients.dCl_dq) ||
                       is_a_dependent_variable(coefficients.dCl_dr) ||
                       is_a_dependent_variable(coefficients.dCm_dp) ||
                       is_a_dependent_variable(coefficients.dCm_dr) ||
                       is_a_dependent_variable(coefficients.dCn_dp) ||
                       is_a_dependent_variable(coefficients.dCn_dq));

  uncertainty_data_present = (
                       is_a_dependent_variable(uncertainty_ref.dCl_dq) ||
                       is_a_dependent_variable(uncertainty_ref.dCl_dr) ||
                       is_a_dependent_variable(uncertainty_ref.dCm_dp) ||
                       is_a_dependent_variable(uncertainty_ref.dCm_dr) ||
                       is_a_dependent_variable(uncertainty_ref.dCn_dp) ||
                       is_a_dependent_variable(uncertainty_ref.dCn_dq));

  aero_damping_off_diag_in_table = verify_aero_damping("off-diagonal");
}

/*******************************************************************************
verify_aero_damping
Purpose:()
*******************************************************************************/
AeroTableSetBase::AeroDampingType
AeroTableSetBase::verify_aero_damping( std::string type)
{
  if (coef_data_present) {
    if (!uncertainty_data_present) {
      CMLMessage::warn(
        __FILE__, __LINE__, "Incomplete data setup\n",
        "Aero damping is provided with table data for the ", type, " coefficients,\n"
        "but none for the uncertainties. Dispersions will be applied based\n"
        "on the values set in the uncertainty structure in the\n"
        "TableLookupSet instance. If these values are not set manually,\n"
        "there will be no dispersions applied.\n");
      return CoeffsOnly;
    }
    else {
      return CoeffsUnc;
    }
  }
  return NotInTable;
}

/*******************************************************************************
set_table_type
Purpose: (Sets the table-type, ensuring that it does not change after all the
          initialization checks have been made.)
*******************************************************************************/
void
AeroTableSetBase::set_table_type( AeroDataTableType new_type)
{
  // trivial-case
  if (new_type == data_table_type) {
    CMLMessage::inform(
      __FILE__,__LINE__,"Redundant initialization sequence.\n",
      "The data-table-type (", new_type, ") already matches the newly specified type.\n"
      "No action taken.\n");
    return;
  }

  if (initialized) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid initialization sequence.\n",
      "The data-table-type cannot be changed after the table has been "
      "initialized.\nChange to type ", new_type, " failed; table remains at type ", data_table_type, "\n");
  }
  else {
    data_table_type = new_type;
  }
}