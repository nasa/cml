/*******************************************************************************

PURPOSE:
   (Provides the template specializations of the method query_equals)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/

#include "cml/models/utilities/math_utils/include/math_utils.hh" // MathUtils
#include "../include/constraint_test_templates.hh"

/*****************************************************************************
Template specializations
Purpose:
  The default query_equals method tests whether variable == threshold.
  This is a bad idea for floating-point variables (float, double), so the
  query_equals method is specialized for the double and float versions of the
  ConstraintTest_Threshold class-template such that variable is equal to
  threshold if they are within equality_threshold of each other.
*****************************************************************************/
template<>
bool ConstraintTest_Threshold<double>::query_equals( double variable)
{
  return (MathUtils::is_within_range( variable,
                                      threshold,
                                      equality_threshold));
}

template<>
bool ConstraintTest_Threshold<float>::query_equals( float variable)
{
  return (MathUtils::is_within_range( static_cast<double>(variable),
                                      static_cast<double>(threshold),
                                      equality_threshold));
}
