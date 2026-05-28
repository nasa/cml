/*******************************************************************************

PURPOSE:
   (Provides the template specializations of the method query_equals)

PROGRAMMERS:
 (((Gary Turner) (OSR) (Jun 2023) (ANTARES)
   (New, based on GNC_PAR grok_duration_check and grok-violation models))
 )
*******************************************************************************/
#include "../include/constraint_test_timed_templates.hh"
#include <cmath> //abs

/*****************************************************************************
Template specializations
Purpose:
  The default query_equals method tests whether variable == threshold.
  This is a bad idea for floating-point variables (float, double), so the
  query_equals method is specialized for the double and float versions of the
  ConstraintTest_ThresholdTimed class-template such that variable is equal to
  threshold if they are within equality_threshold of each other.
*****************************************************************************/
template<>
bool ConstraintTest_ThresholdTimed<double>::query_equals( double variable)
{
  return (std::abs(variable-threshold) <= equality_threshold);
}

template<>
bool ConstraintTest_ThresholdTimed<float>::query_equals( float variable)
{
  return (std::abs(variable-threshold) <= equality_threshold);
}
