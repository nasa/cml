/*******************************************************************************

PURPOSE:
   (Provides the template specializations of the method query_equals)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES) (Initial version))
 )
*******************************************************************************/

#include "../include/constraint_test_templates.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

/**
 * Template specialization for checking the equality of `double`s
 */
template<>
bool ConstraintTest_Threshold<double>::query_equals( double variable) const
{
  return MathUtils::is_within_abs_tolerance( variable,
                                             threshold,
                                             equality_threshold);
}

/**
 * Template specialization for checking the equality of `float`s
 */
template<>
bool ConstraintTest_Threshold<float>::query_equals( float variable) const
{
  return MathUtils::is_within_abs_tolerance( static_cast<double>(variable),
                                             static_cast<double>(threshold),
                                             equality_threshold);
}
