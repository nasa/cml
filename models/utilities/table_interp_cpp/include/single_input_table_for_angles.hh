/*******************************************************************************
PURPOSE:
  (Table lookup model for a special case where the dependent variable(s)
  represent angles and consequently may wrap around.  This table-type is
  restricted to a single independent variable but may still have multiple
  dependent angular variables)

ASSUMPTIONS:
  (All dependent variables are angles, and are either all degrees or all
  radians, no mixing.)

LIMITATIONS:
  (Limited to a single independent variable.  Interpolation across
  multiple wraps is not universally defined.)

 LIBRARY DEPENDENCY:
    ((../src/single_input_table_for_angles.cc)
    )

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2014) (New implementation for C++ models))
   ((Bingquan Wang) (OSR) (August 2014) (IV&V cleanup and refactored))
  )
*******************************************************************************/
#ifndef ANTARES_SINGLE_INPUT_TABLE_FOR_ANGLES_HH
#define ANTARES_SINGLE_INPUT_TABLE_FOR_ANGLES_HH

#include <cstdarg> // for variadic function arguments

#include "generic_single_input_table.hh"

class SingleInputTableForAngles : public GenericSingleInputTable
{
 public:
  bool output_in_radians; /* (--)
       Flag indicating that the output data will be interpreted as radians.
       Set to false for degrees.  Default: true (radians). */
 protected:
  double half_circle; /* (--)
      Value of a half-circle in either radians or degrees, depending oni
      the setting output_in_radians. */
 public:
  SingleInputTableForAngles( bool output_in_radians = true);
  SingleInputTableForAngles( double *dependent_variables,
                             size_t num_vars,
                             bool output_in_radians = true);
  SingleInputTableForAngles( double & dependent_var,
                             bool output_in_radians = true);
  SingleInputTableForAngles( const DoublePtrVec & dependent_variables,
                             bool output_in_radians = true);
  virtual ~SingleInputTableForAngles() {};

  virtual bool initialize() override;
 protected:
  virtual bool generate_output() override;
  void bound_to_half_circle( double & angle);
 private:
  // Disable the copy/assignment operators
  SingleInputTableForAngles (const SingleInputTableForAngles&);
  SingleInputTableForAngles& operator = (const SingleInputTableForAngles&);
};
#endif
