/*******************************TRICK HEADER******************************
PURPOSE:
 (Provides an extension to WatchValuesDelay, extending the
  triggering capabilities)

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2023) (ANTARES)
   (Based on logic in DATA_COLLECTION_TRIGGER and DC_TRIGGER_PRIMAL,
    after stripping out the risky parts and picking up the existingi
    capabilities in CML))
  )
**********************************************************************/
#include "../include/event_trigger.hh"



/*****************************************************************************
Constructor
*****************************************************************************/
EventTriggerBase::EventTriggerBase()
    :
    comparison_logic(Undefined),
    function_modifier(NoModification),
    direction_limit(NoDirectionLimit),
    variable_dbl(0.0),
    variable_dbl_prev(0.0),
    variable_dbl_activation(0.0),
    reference_dbl(0.0),
    locked(false)
{}


/*****************************************************************************
apply_function_modifier
Purpose:
  Take the incoming variable-value and apply the function-modifier
  prior to testing against the threshold reference-value
***************************************************************************/
void
EventTriggerBase::apply_function_modifier()
{
  switch (function_modifier) {
  default:
    break;
  case Absolute:
    variable_dbl = std::abs( variable_dbl);
    break;
  case Sine:
    variable_dbl = std::sin( variable_dbl);
    break;
  case Cosine:
    variable_dbl = std::cos( variable_dbl);
    break;
  case Tan:
    variable_dbl = std::tan( variable_dbl);
    break;
  case AbsSine:
    variable_dbl = std::abs( std::sin( variable_dbl));
    break;
  case AbsCosine:
    variable_dbl = std::abs( std::cos( variable_dbl));
    break;
  case AbsTan:
    variable_dbl = std::abs( std::tan( variable_dbl));
    break;
  }
}

/*****************************************************************************
has_conditional_reference
Purpose:
  Returns a simple flag indicating whether the trigger has a
  conditionally-assigned reference.
*****************************************************************************/
bool
EventTriggerBase::has_conditional_reference()
{
  return ((comparison_logic == MAX_CONDITIONAL_FIRST) ||
          (comparison_logic == MIN_CONDITIONAL_FIRST) ||
          (comparison_logic == ABS_MAX_CONDITIONAL_FIRST) ||
          (comparison_logic == ABS_MIN_CONDITIONAL_FIRST) ||
          (comparison_logic == MAX_CONDITIONAL_LAST) ||
          (comparison_logic == MIN_CONDITIONAL_LAST) ||
          (comparison_logic == ABS_MAX_CONDITIONAL_LAST) ||
          (comparison_logic == ABS_MIN_CONDITIONAL_LAST));
}

/*****************************************************************************
set_new_reference
Purpose:
  For events with a conditional reference, resets the reference when
  the overall event-set has met its conditions.
*****************************************************************************/
void
EventTriggerBase::set_new_reference()
{
  switch(comparison_logic) {
  case MAX_CONDITIONAL_FIRST:
  case MIN_CONDITIONAL_FIRST:
  case MAX_CONDITIONAL_LAST:
  case MIN_CONDITIONAL_LAST:
    reference_dbl = variable_dbl;
    break;
  case ABS_MAX_CONDITIONAL_FIRST:
  case ABS_MIN_CONDITIONAL_FIRST:
  case ABS_MAX_CONDITIONAL_LAST:
  case ABS_MIN_CONDITIONAL_LAST:
    reference_dbl = std::abs(variable_dbl);
    break;
  default:
    // No-op
    break;
  }
}

