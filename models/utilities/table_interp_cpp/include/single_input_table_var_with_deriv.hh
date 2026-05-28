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

 LIBRARY DEPENDENCY:
    ((../src/single_input_table_var_with_deriv.cc)
    )

 PROGRAMMERS:
   (((Gary Turner) (OSR) (May 2023) (Antares) (initial version))
   )
*******************************************************************************/
#ifndef ANTARES_SINGLE_INPUT_TABLE_FOR_VAR_DERIV_HH
#define ANTARES_SINGLE_INPUT_TABLE_FOR_VAR_DERIV_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "generic_single_input_table.hh"

class SingleInputTableVarDeriv : public GenericSingleInputTable
{
 public:
  GenericSingleInputTable  derivs; /* (--)
    A parallel instance of GenericSingleInputTable used to manage the data
    for the first-derivative variable.*/
  bool omit_derivative_vals; /* (--)
    Flag indicating whether this dual-data table is to be used to populate
    the first-derivative variables as well as the zeroth-derivative
    variables. Default: false (populate both)*/
  bool use_linear_interpolation; /* (--)
    Flag indicating that simple linear interpolation should be used for
    both the zeroth-derivative and first-derivative (if
    populate_derivative_vals) variables, independent of each other.
    This effectively reverts the model to being two independent instances of
    GenericSingleInputTable.*/


  // Constructors -- inherit everything fom GenericSingleInputTable
  // (from GenericMultiInputTable) and add more:
  // Note -- if using the ones from GenericSingleInputTable, the member
  // derivs will be constructed using the default GenericSingleInputTable
  // constructor; the new constructors provide construction values to derivs.
  using GenericSingleInputTable::GenericSingleInputTable;
  SingleInputTableVarDeriv();
  SingleInputTableVarDeriv( double *dependent_variables,
                            double *dependent_variables_derivs,
                            size_t num_vars);
  SingleInputTableVarDeriv( double & dependent_var,
                            double & dependent_var_deriv);
  SingleInputTableVarDeriv( const DoublePtrVec & dependent_variables,
                            const DoublePtrVec & dependent_variables_deriv);

  virtual bool initialize() override;
  protected:
  virtual bool generate_output() override;
 private:
  // Disable the copy/assignment operators
  SingleInputTableVarDeriv (const SingleInputTableVarDeriv&);
  SingleInputTableVarDeriv& operator = (const SingleInputTableVarDeriv&);
};
#endif
