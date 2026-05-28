/*******************************TRICK HEADER******************************
PURPOSE:
 (Provides an extension to WatchValuesDelay, extending the
  triggering capabilities)

LIBRARY DEPENDENCY:
  ((../src/event_trigger.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (October 2023) (ANTARES)
   (Based on logic in DATA_COLLECTION_TRIGGER and DC_TRIGGER_PRIMAL,
    after stripping out the risky parts and picking up the existingi
    capabilities in CML))
  )
**********************************************************************/
#ifndef CML_EVENT_MANAGEMENT_EVENT_TRIGGER_HH
#define CML_EVENT_MANAGEMENT_EVENT_TRIGGER_HH

#include "cml/models/vehicle_management/events_manager/include/watch_values_delay.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

// Provide SWIG with the necessary template so it can see through to the
// layer above.
#ifdef SWIG
#ifndef CML_EVENT_TRIGGERS_SWIG_TEMPLATES
#define CML_EVENT_TRIGGERS_SWIG_TEMPLATES
%template(WatchValuesDelay_double) WatchValuesDelay<double>;
#endif
#endif

/*****************************************************************************
EventTriggeBaser
Purpose:
  Provide a definition of the enumerations used in the class template
  EventTrigger; separating them into a standalone class allows them to be
  used without having to specify the template parameter.
*****************************************************************************/
struct EventTriggerBase
{
  enum TriggerCondition
  {
    Undefined,
    EQ,
    NE,
    GT,
    LT,
    GE,
    LE,
    ABS_EQ,
    ABS_GT,
    ABS_GE,
    ABS_LT,
    ABS_LE,
    OC,
    Change,
    Crossing,
    // options with managed values of reference:
    MAX_FIRST,  // triggers when variable reaches a pt > reference (FIRST occurance)
                // and resets reference to this value.
    MIN_FIRST,
    ABS_MAX_FIRST,
    ABS_MIN_FIRST,
    MAX_CONDITIONAL_FIRST,    // triggers when variable reaches a pt > reference and
                              // resets reference to this value only if the event is
                              // triggered.
    MIN_CONDITIONAL_FIRST,
    ABS_MAX_CONDITIONAL_FIRST,
    ABS_MIN_CONDITIONAL_FIRST,
    
    // options with managed values of reference:
    MAX_LAST,   // triggers when variable reaches a pt >= reference (LAST occurance)
                // and resets reference to this value.
    MIN_LAST,
    ABS_MAX_LAST,
    ABS_MIN_LAST,
    MAX_CONDITIONAL_LAST,     // triggers when variable reaches a pt >= reference and
                              // resets reference to this value only if the event is
                              // triggered.
    MIN_CONDITIONAL_LAST,
    ABS_MAX_CONDITIONAL_LAST,
    ABS_MIN_CONDITIONAL_LAST
  } comparison_logic; /* (--)
    Specifies how to compare the value of variable against the
    recorded reference value.*/

  enum FunctionModifier
  {
    NoModification,
    Absolute,
    Sine,
    Cosine,
    Tan,
    AbsSine,
    AbsCosine,
    AbsTan
  } function_modifier; /* (--)
    Specifies how to modify the value of "variable" before comparing against
    "reference". */

  enum DirectionLimit
  {
    NoDirectionLimit                = 0,
    NA                              = 0,
    Increasing                      = 1,
    INCREASING                      = 1,
    INCR                            = 1,
    INC                             = 1,
    Decreasing                      = 2,
    DECREASING                      = 2,
    DECR                            = 2,
    DEC                             = 2,
    Strictly_Increasing             = 3,
    Strictly_Decreasing             = 4,
  } direction_limit; /* (--)
    Specifies further restrictions on what is considered a triggering.
    E.g. value has to be greater than threshold and decreasing.
    Using all legacy enumerations to try to maintain as much
    backward-compatibility as possible.*/
 protected:
  double variable_dbl; /* (--)
    The value obtained when the template pointer variable is dereferenced
    and cast to type double. This value may be further modified by a
    function in the enumerated list FunctionModifier.*/
  double variable_dbl_prev; /* (--)
    The previous value of variable_dbl.*/
  double variable_dbl_activation; /* (--)
    The value of variable_dbl at activation, used in cases of the reference
    threshold being assigned as an offset from the initial (activation)
    value.*/
  double reference_dbl; /* (--)
    The value obtained when the template reference value (or template
    reference pointer) is cast to type double. This defines the threshold
    for logical comparisons made in this class.*/
  bool locked; /* (--)
    Boolean used to lock at each cycle, so each WatchValuesBase cannot be 
    tested more than once per cycle, leading to unintended behavior.*/

 public:
  EventTriggerBase();
  void apply_function_modifier();
  bool has_conditional_reference();
  void set_new_reference();
};

/*****************************************************************************
EventTrigger
Purpose: A templated extension of EventTriggerBase
*****************************************************************************/
template <typename T>
class EventTrigger : public WatchValuesDelay<T>,
                     public EventTriggerBase
{
  bool relative_to_activation_ET; /* (--)
    This is a bit kludgy. relative_to_activation is inherited from
    WatchValuesDelay (from WatchValuesBase) and is a public flag;
    for an EventTrigger, this needs to be locked down at activation.
    relative_to_activation_protected is also inherited from WatchValuesDelay
    and is set at activation.
    But this protected variable is used in WatchValuesBase::test_crossing()
    to reset "reference" based on the current value of variable.
    We need to reset "reference" based on the current value of "variable"
    after the modifier function has been applied.
    So we cannot use relative_to_activation_protected and need a
    new variable, similar in intent, specifically for EventTrigger.*/

 public:
  EventTrigger( const double & delay_ref,
                std::string name_ = "")
    :
    WatchValuesDelay<T> (delay_ref),
    relative_to_activation_ET(false)
  {
    /* name is inherited from WatchValuesDelay from WatchValuesBase from
    * WatchValuesBaseCore.*/
    this->name = name_;
    /* In the event that type T is not float or similar, we need the
     * use_threshold_crossing_trigger flag to bring the execution back to this
     * class's test_crossing_dbl() method from high up in the WatchValues
     * inheritance tree.*/
    WatchValuesBase<T>::use_threshold_crossing_trigger = true;
  };
  virtual ~EventTrigger(){};


 protected:
/*****************************************************************************
test_crossing_dbl
Purpose:
  This method determines whether the trigger is marked as triggered.
  The name derives from the original simple WatchValues base-class where if
  the threshold was crossed, the event was triggered.
  This method is a bit more complicated than that, so the name is a bit of a
  misnomer but it is retained so this type can be used alongside simple
  WatchValues implementations.
Notes:
- Going all the way back to WatchValuesBaseCore, the method test_crossing()
  is the method-template entry-point for determining whether the event
  should be triggered. For data types double, float, and any others with
  the "use_threshold_crossing_trigger" flag set, the call to test_crossing
  eventually transfers to test_crossing_dbl(double), which evaluates
  whether the value is on the "triggered" side of the threshold, and sets
  the event_triggered flag in response.
- In WatchValuesDelay, the test_crossing() method is expanded to wrap
  the delay mechanism around the core WatchValuesBaseCore::test_crossing_dbl
  functionality.
- In this model, we utilize the wrapped-up capabilities provided by
  WatchValuesDelay::test_crossing and overwrite the basic
  threshold-crossing evaluations in WatchValuesBaseCore::test_crossing_dbl
  to include the new requirements encapsulated in the enumerations
  TriggerCondition, FunctionModifier, and DirectionLimit.
- The reference (threshold) value can be:
   - Fixed: pre-defined to a specific value
   - Fixed: computed at trigger-activations to be a pre-defined offset from
       the value of the monitored variable at trigger-activation.
   - Variable: using a pointer to access the reference-variable.
   - Variable: using that pointer to access the extent to which the
       reference (threshold) value should be offset from the recorded baseline
       value of the monitored variable at trigger-activation.
  Case 1, and 2 use the class member value reference_dbl.
  Case 3 uses the passed argument ref_val.
  Case 4 uses ref_val + the value stored at activation, which is a
  post-modified value unknown to WatchValuesBase so cannot be incorporated
  in WatchValuesBase prior to entry into this method.
******************************************************************************/
  void test_crossing_dbl( double var_val,
                          double ref_val)
  {
    if (this->reference_is_variable) {
      if (relative_to_activation_ET) {
        reference_dbl = ref_val + variable_dbl_activation;
      } else {
        reference_dbl = ref_val;
      }
    }

    variable_dbl = var_val;
    /* Take the incoming variable-value and apply the function-modifier
     * prior to testing against the threshold reference-value.*/
    apply_function_modifier();

    /* Apply the more detailed threshold comparison logic options to determine
     * whether the event is triggered from the perspective of purely numerical
     * comparison of variable-value against threshold.*/
    switch (comparison_logic) {
    default:
      CMLMessage::error(__FILE__,__LINE__,
        "Invalid comparator specified : ", comparison_logic,".\n"
        "Disabling trigger.\n");
        this->event_triggered = false;
        this->deactivate();
      return;
    case EQ:
      // FIXME the use of is_near_equal() in this and other cases in this method,
      // with the default ulp of 0.5, tests for actual equality to the precision of
      // the representation. 
      this->event_triggered = MathUtils::is_near_equal(variable_dbl, reference_dbl);
      break;
    case NE:
      this->event_triggered = !MathUtils::is_near_equal(variable_dbl, reference_dbl);
      break;
    case GT:
    case MAX_CONDITIONAL_FIRST:
      this->event_triggered = (variable_dbl > reference_dbl);
      break;
    case LT:
    case MIN_CONDITIONAL_FIRST:
      this->event_triggered = (variable_dbl < reference_dbl);
      break;
    case GE:
    case MAX_CONDITIONAL_LAST:
      this->event_triggered = (variable_dbl >= reference_dbl);
      break;
    case LE:
    case MIN_CONDITIONAL_LAST:
      this->event_triggered = (variable_dbl <= reference_dbl);
      break;
    case ABS_EQ:
      this->event_triggered = MathUtils::is_near_equal( std::abs(variable_dbl),
                                                       std::abs(reference_dbl));
      break;
    case ABS_GT:
    case ABS_MAX_CONDITIONAL_FIRST:
      this->event_triggered = (std::abs(variable_dbl) >
                               std::abs(reference_dbl));
      break;
    case ABS_LT:
    case ABS_MIN_CONDITIONAL_FIRST:
      this->event_triggered = (std::abs(variable_dbl) <
                               std::abs(reference_dbl));
      break;
    case ABS_GE:
    case ABS_MAX_CONDITIONAL_LAST:
      this->event_triggered = (std::abs(variable_dbl) >=
                               std::abs(reference_dbl));
      break;
    case ABS_LE:
    case ABS_MIN_CONDITIONAL_LAST:
      this->event_triggered = (std::abs(variable_dbl) <=
                               std::abs(reference_dbl));
      break;
    case OC:
    case Change:
      this->event_triggered = MathUtils::has_changed_from(variable_dbl, variable_dbl_prev);
      break;
    case Crossing:
    {
      /* If the delta between variable_dbl and reference_dbl is non-zero, evaluate
       * whether the sign has changed.*/
      double this_delta =  variable_dbl - reference_dbl;
      if ( MathUtils::is_near_equal( this_delta, 0.0)) {
        this->event_triggered = false;
      }
      else {
        this->event_triggered = (this_delta * this->delta_record < 0);
        this->delta_record = this_delta;
      }
      break;
    }
    case MAX_FIRST:
      this->event_triggered = (variable_dbl > reference_dbl);
      if (this->event_triggered) {
        reference_dbl = variable_dbl;
      }
      break;
    case MIN_FIRST:
      this->event_triggered = (variable_dbl < reference_dbl);
      if (this->event_triggered) {
        reference_dbl = variable_dbl;
      }
      break;
    case ABS_MAX_FIRST:
      this->event_triggered = (std::abs(variable_dbl) > reference_dbl);
      if (this->event_triggered) {
        reference_dbl = std::abs(variable_dbl);
      }
      break;
    case ABS_MIN_FIRST:
      this->event_triggered = (std::abs(variable_dbl) < reference_dbl);
      if (this->event_triggered) {
        reference_dbl = std::abs(variable_dbl);
      }
      break;
    case MAX_LAST:
      this->event_triggered = (variable_dbl >= reference_dbl);
      if (this->event_triggered) {
        reference_dbl = variable_dbl;
      }
      break;
    case MIN_LAST:
      this->event_triggered = (variable_dbl <= reference_dbl);
      if (this->event_triggered) {
        reference_dbl = variable_dbl;
      }
      break;
    case ABS_MAX_LAST:
      this->event_triggered = (std::abs(variable_dbl) >= reference_dbl);
      if (this->event_triggered) {
        reference_dbl = std::abs(variable_dbl);
      }
      break;
    case ABS_MIN_LAST:
      this->event_triggered = (std::abs(variable_dbl) <= reference_dbl);
      if (this->event_triggered) {
        reference_dbl = std::abs(variable_dbl);
      }
      break;
    }

    /* Evaluate and apply the direction limitation if the event has been
     * triggered from the perspective of a purely numerical comparison (if it
     * hasn't already been triggered, considering the direction would be
     * moot).
     * Note that "increasing" really means "not decreasing" and
     * "decreasing" really means "not increasing".*/
    if (this->event_triggered) {
      switch(direction_limit) {
      default: // including NoDirectionLimit, no consideration applied.
        break;
      case Increasing:
        this->event_triggered = (variable_dbl >= variable_dbl_prev);
        break;
      case Decreasing:
        this->event_triggered = (variable_dbl <= variable_dbl_prev);
        break;
      case Strictly_Increasing:
        this->event_triggered = (variable_dbl > variable_dbl_prev);
        break;
      case Strictly_Decreasing:
        this->event_triggered = (variable_dbl < variable_dbl_prev);
        break;
      }
    }
    variable_dbl_prev = variable_dbl;
  }

/*****************************************************************************
activate
Purpose:
  Completes the activation sequence, including verification that the
  appropriate data and pointers have been set in a compatible manner.)
Notes:
- Completely replacing WatchValuesBase::activate() because while the nullptr
  check and relative_to_activation modification are still needed, we need to
  inject the modifiy_variable() call between them. Also the
  add-self-to-manager is not needed for this type of trigger -- it does not
  get added to the manager.
- Using a relative-to-activation setting and applying a function-modifier
   to the variable-value could be interpreted in either order. In this case,
   we are setting the threshold as some delta away from the modified-value.
   I.e. we apply the function-modifier and then the delta:
      Threshold value = f(v_0) + delta
   So then we will be testing, e.g: f(v) > f(v_0) + delta
 ******************************************************************************/
  void activate() {
    if (this->variable == nullptr) {
      CMLMessage::error ( __FILE__, __LINE__,
        "Invalid Event/Watch activation: nullptr variable:\n",
        "An attempt was made to activate an EventTrigger instance with a\n "
        "nullptr variable to watch.\n  Attempt failed.\n");
      return;
    }

    if (comparison_logic == Undefined) {
      CMLMessage::error(__FILE__,__LINE__,
        "Invalid comparator. Comparator remains on the default 'Undefined'.\n"
        "There is no comparison available to result in a triggering.\n"
        "Trigger activation failed.\n");
      return;
    }

    if ((this->reference_is_variable ||
         this->relative_to_activation) &&
        ((comparison_logic == MAX_FIRST) ||
         (comparison_logic == MIN_FIRST) ||
         (comparison_logic == ABS_MAX_FIRST) ||
         (comparison_logic == ABS_MIN_FIRST) ||
         (comparison_logic == MAX_LAST) ||
         (comparison_logic == MIN_LAST) ||
         (comparison_logic == ABS_MAX_LAST) ||
         (comparison_logic == ABS_MIN_LAST) ||
         (comparison_logic == MAX_CONDITIONAL_FIRST) ||
         (comparison_logic == MIN_CONDITIONAL_FIRST) ||
         (comparison_logic == ABS_MAX_CONDITIONAL_FIRST) ||
         (comparison_logic == ABS_MIN_CONDITIONAL_FIRST) ||
         (comparison_logic == MAX_CONDITIONAL_LAST) ||
         (comparison_logic == MIN_CONDITIONAL_LAST) ||
         (comparison_logic == ABS_MAX_CONDITIONAL_LAST) ||
         (comparison_logic == ABS_MIN_CONDITIONAL_LAST))) {
      CMLMessage::error(__FILE__,__LINE__,
        "Invalid configuration. The reference-management flags:\n"
        "         reference_is_variable:  ",this->reference_is_variable,"\n"
        "         relative_to_activation: ",this->relative_to_activation,"\n"
        "should be left at their default false when any of the following "
        "comparators is used:\n"
        "     {MAX_FIRST, MIN_FIRST, ABS_MAX_FIRST, ABS_MIN_FIRST, MAX_LAST,\n"
        "      MIN_LAST, ABS_MAX_LAST, ABS_MIN_LAST, MAX_CONDITIONAL_FIRST,"
        "      MIN_CONDITIONAL_FIRST, ABS_MAX_CONDITIONAL_FIRST, ABS_MIN_CONDITIONAL_FIRST,"
        "      MAX_CONDITIONAL_LAST, MIN_CONDITIONAL_LAST, ABS_MAX_CONDITIONAL_LAST,"
        "      ABS_MIN_CONDITIONAL_LAST}\n"
        "These comparators require managing the reference, it cannot be a "
        "variable or computed relative to activation value.\n");
      return;
    }



    // Compute and store the function-modified value of the monitored variable:
    variable_dbl = static_cast<double>(*(this->variable));
    apply_function_modifier();
    variable_dbl_prev = variable_dbl;

    // Compute and store the value of the reference (threshold):
    if ((comparison_logic == MAX_FIRST) ||
        (comparison_logic == MAX_LAST) ||
        (comparison_logic == MIN_FIRST) ||    
        (comparison_logic == MIN_LAST)) {
      reference_dbl = variable_dbl;
    }
    else if ((comparison_logic == ABS_MAX_FIRST) ||
             (comparison_logic == ABS_MAX_LAST) ||
             (comparison_logic == ABS_MIN_FIRST) ||
             (comparison_logic == ABS_MIN_LAST)) {
      reference_dbl = std::abs(variable_dbl);
    }
    else if ((comparison_logic == MAX_CONDITIONAL_FIRST) || 
             (comparison_logic == MAX_CONDITIONAL_LAST)) {
      reference_dbl = std::numeric_limits<double>::lowest();
    }
    else if ((comparison_logic == MIN_CONDITIONAL_FIRST) || 
             (comparison_logic == MIN_CONDITIONAL_LAST) ||
             (comparison_logic == ABS_MIN_CONDITIONAL_FIRST) || 
             (comparison_logic == ABS_MIN_CONDITIONAL_LAST)) {
      reference_dbl = std::numeric_limits<double>::max();
    }
    else if ((comparison_logic == ABS_MAX_CONDITIONAL_FIRST) || 
             (comparison_logic == ABS_MAX_CONDITIONAL_LAST)) {
      reference_dbl = 0.0;
    }
    else if (this->reference_is_variable) {
      /* Note -- reference_is_variable is protected and only set to true if
       * reference_ptr is not NULL.*/
      reference_dbl = static_cast<double> (*(this->reference_ptr));
    } else {
      reference_dbl = static_cast<double> (this->reference);
    }

    // Adjust the reference value, incrementing it onto the initial value of
    // the variable if desired.
    if (this->relative_to_activation) {
      /* We will be using relative_to_activation_ET instead of
       * relative_to_activation_protected, and blocking the WatchValuesBase
       * code that uses relative_to_activation_protected by setting it to
       * false.*/
      relative_to_activation_ET = true;
      this->relative_to_activation_protected = false;
      this->variable_at_activation = *(this->variable);
      variable_dbl_activation = variable_dbl;
      reference_dbl += variable_dbl;
    }

    // All actions complete, set the active flag and exit.
    this->active = true;
  };

 private:
  EventTrigger (const EventTrigger& rhs);
  EventTrigger& operator = (const EventTrigger& rhs);
};
#endif
