/*******************************TRICK HEADER***********************************
 PURPOSE:
    ( Provide a coupled interpolation algorithm for a variable and its
      derivative. The variable is fit to a cubic polynomial such that its
      value and that of its derivative are continuous at both ends of the
      interval -- a linear interpolation does not provide continuity in
      the deriviative at the end of the interval.
      The derivative value is computed fom the derivative of the same
      cubic polynomial so also depends on the values of the data for the
      base-variable.)

 PROGRAMMERS:
   (((Gary Turner) (OSR) (May 2023) (Antares) (initial version))
   )
*******************************************************************************/
#include "../include/single_input_table_var_with_deriv.hh"


/*****************************************************************************
Constructor
Notes: - Class inherits constructors from GenericSingleInputTable
         (and from GenericMultiInputTable), these are additional
       - if using the ones from GenericSingleInputTable, the member
         derivs will be constructed using the default GenericSingleInputTable
         constructor; these new constructors provide construction values
         to the derivs member.
*****************************************************************************/
SingleInputTableVarDeriv::SingleInputTableVarDeriv()
  :
  GenericSingleInputTable(),
  derivs(),
  omit_derivative_vals(false),
  use_linear_interpolation(false)
{}
/****************************************************************************/
SingleInputTableVarDeriv::SingleInputTableVarDeriv(
    double *dependent_variables,
    double *dependent_variables_derivs,
    size_t num_vars)
  :
  GenericSingleInputTable( dependent_variables,
                           num_vars),
  derivs( dependent_variables_derivs,
          num_vars),
  omit_derivative_vals(false),
  use_linear_interpolation(false)

{}
/****************************************************************************/
SingleInputTableVarDeriv::SingleInputTableVarDeriv(
    double & dependent_variable,
    double & dependent_variable_derivs)
  :
  GenericSingleInputTable( dependent_variable),
  derivs(dependent_variable_derivs),
  omit_derivative_vals(false),
  use_linear_interpolation(false)

{}

/****************************************************************************/
SingleInputTableVarDeriv::SingleInputTableVarDeriv(
    const DoublePtrVec & dependent_variables,
    const DoublePtrVec & dependent_variables_deriv)
  :
  GenericSingleInputTable( dependent_variables),
  derivs( dependent_variables_deriv),
  omit_derivative_vals(false),
  use_linear_interpolation(false)
{}


/*****************************************************************************
initialize
Purpose:
  Override of the GenericSingleInputTable::initialize() method,
  extending it by making some additional checks.
*****************************************************************************/
bool
SingleInputTableVarDeriv::initialize()
{
  // Run the primary initialization first to check that independents[0] is
  // valid.
  if (!GenericSingleInputTable::initialize()) {
    return false;
  }

  // Give derivs a copy of the independent variable so that we can
  // initialize it without fault. It doesn't need it.
  if (derivs.independents.empty()) {
    derivs.add_independent( *independents[0].first,
                            independents[0].second);
  } else if (derivs.independents[0].first != independents[0].first) {
    CMLMessage::error(__FILE__,__LINE__,
      "Initialization error: The variable+derivative coupled\n"
      "table-interpolation routine requires that the variable and its\n"
      "derivative-variable use the same Independent Variable.\n"
      "Recommended practice is to let the model auto-assign the\n"
      "Independent Variable to the derivative-variable lookup.\n"
      "An independent variable was manually assigned, and it is not\n"
      "the same as the one used for the variable.\n"
      "Initialization failed.\nTable is not usable.\n");
    return false;
  }
  // else -- already correctly assigned, no action needed.

  // Now initialize the derivs table.
  derivs.initialize();
  if (!derivs.initialized) {
    CMLMessage::error( __FILE__,__LINE__,
      "Unable to initialize the GenericSingleInputTable representing the "
      "first-derivative values.\nInitialization failed.\n"
      "Table is not usable.\n");
    return false;
  }

  // Check that there is a derivative variable for each regular variable
  if (output.size() != derivs.output.size()) {
    CMLMessage::error( __FILE__,__LINE__,
      "Initialization error: Table is configured to populate:\n",
      output.size(), " zeroth-derivative variables, and\n",
      derivs.output.size(), " first-derivative variables.\n"
      "These numbers should be identical.\n"
      "Initialization failed.\nTable is not usable.\n");
    return false;
  }

  // All checks passed:
  return true;
}


/*****************************************************************************
generate_output
Purpose:
  Override of the GenericSingleInputTable::generate_output() method
  to conduct the lookup / interpolation on both the zeroth-derivative
  and first-derivative variable(s).
  -  If using a lookup or if interpolating and off-table, or if the model
     is configured to force linear interpolation, this just forwards to
     GenericSingleInputTable.
  -  If configured to skip the first-derivative values, bypass that
     table's update completely.
*****************************************************************************/
bool
SingleInputTableVarDeriv::generate_output()
{
  // If there is no interpolation, or instruction to use only linear
  // interpolation for each variable, just use the inherited
  // GenericSingleINputTable methods.
  if (use_linear_interpolation ||
      data_point_index.size() == 1) {
    // ignore return value, it is always true
    GenericSingleInputTable::generate_output();

    if (!omit_derivative_vals) {
      derivs.data_point_index  = data_point_index;
      derivs.generate_output();
    }
    return true;
  }

  // else, using interpolation and cross-linking of zeroth- and first-
  // derivatives.
  // make convenient local copies of the value f (fraction), 1-f and the
  // size of the current bin in the independent variable table
  // NOTE -- it is assumed that the derivative of the
  //         dependent variable is taken with respect to
  //         the independent variable, so the bin-size of the
  //         independent variable is the step size dx in y' = dy / dx
  double f = independents[0].first->fraction;
  double mf = 1-f;
  size_t data_ix = data_point_index[0];
  double bin_size = independents[0].first->data[data_ix+1] -
                    independents[0].first->data[data_ix];

  // for the base variable, weights are:
  // (1-f)^2 (1+2f)         applied to x_0
  // f^2 (3 - 2f)           applied to x_1
  // bin_size * f (1-f)^2   applied to xdot_0
  // - bin_size * f^2 (1-f) applied to xdot_1
  for (double* out_ptr : output) {
    *out_ptr = data[data_ix]          * mf * mf * (1 + 2*f) +
               data[data_ix+1]        * f * f * (3 - 2*f)     +
               bin_size * (
                 derivs.data[data_ix]   * f * mf * mf -
                 derivs.data[data_ix+1] * f * f   * mf);
    data_ix += num_data_elements_per_increment_of_index[0];
  }

  if (!omit_derivative_vals) {
    // Calculate the size of the bin in the independent variable.

    // reset data_ix and repeat for the derivative variable
    // For the derivative values, the weightings are:
    // [6f(f-1)] / bin_size applied to x_0
    // [6f(1-f)] / bin_size applied to x_1
    // (1-f) (1-3f)         applied to xdot_0
    // f (3f-2)             applied to xdot_1
    data_ix = data_point_index[0];
    for (double* out_ptr : derivs.output) {
      *out_ptr = (data[data_ix+1] - data[data_ix]) * 6 * f * mf / bin_size +
                 derivs.data[data_ix]   * mf * (1 - 3*f) +
                 derivs.data[data_ix+1] * f  * (3*f - 2);
      data_ix += num_data_elements_per_increment_of_index[0];
    }
  }
  return true;
}
