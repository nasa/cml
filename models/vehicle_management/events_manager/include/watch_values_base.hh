/*******************************************************************************
PURPOSE:
   (Base class of the event-classes, a.k.a. watch-value-* classes)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (October 2014) (Antares))
    ((Bingquan Wang) (OSR) (April, 2017) (Antares) (Fixed the compilation warning
                     of float-point number equality comparison))
   )
 ******************************************************************************/

#ifndef CML_WATCH_VALUES_BASE_HH
#define CML_WATCH_VALUES_BASE_HH

#include <cmath> // abs
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "watch_values_base_core.hh"


/*****************************************************************************
WatchValuesBase
Purpose:(A class template, allows for watching of an undetermined possibility
         of data types.)
NOTE - methods are all defined here and not in .cc because of the nature of
       templates.
*****************************************************************************/
template <typename watchType>
class WatchValuesBase : public WatchValuesBaseCore {

 protected:
  bool reference_is_variable; /* (--) Reference is a variable, not fixed value.*/
  const watchType * variable;       /* (--) variable being watched*/
  watchType   reference;      /* (--) fixed value to test against*/
  const watchType * reference_ptr;  /* (--) variable value to test against.*/
  watchType   variable_at_activation; /* (--) *variable at event activation*/

 public:
  bool use_threshold_crossing_trigger; /* (--)
    Flag to indicate whether to treat a variable with a discrete data type
    (e.g. int and related integer types, enumerations, bools) as a
    continuous variable for the purpose of detecting threshold crossings.
    Default: false (discrete data types will be treated as discrete
    variables and the event trigger on reaching the target value).*/
  WatchValuesBase()
     :
     reference_is_variable(false),
     variable(NULL),
     reference(), // no value; this is of watchType.
     reference_ptr(NULL),
     variable_at_activation(),
     use_threshold_crossing_trigger(false) {};
  virtual ~WatchValuesBase(){};

/*****************************************************************************
set_watch
Purpose:(Sets the variable and the reference against which it will be compared)
*****************************************************************************/
  // ** reference is fixed-value **
  // ************************************************************************
  void set_watch( const watchType & var, watchType   ref) {
    variable = &var;
    reference = ref;
    reference_is_variable = false;
    return;
  };
  /***************************************************************************/
  //  reference is a variable-value
  /***************************************************************************/
  void set_watch( const watchType & var, const watchType * ref) {
    if (ref == nullptr) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot set a watch to monitor a variable against a null reference "
        "variable.\n");
      return;
    }
    variable = &var;
    reference_ptr = ref;
    reference_is_variable = true;
    return;
  };
  /***************************************************************************/
  //     These methods are here for use with primitives only
  //     (e.g. double, int, bool, etc.)
  //     Trick/SWIG is unable to pass references to primitives out of the
  //     input-file user-interface.
  //     These methods, coupled with trick.get_address("<full-variable-name>")
  //     facilitate input-file user-interaction.
  //     This process is inherently unsafe and should be used as a last
  //     resort and only after double checking that the specified variable is
  //     of the correct type.  Type-checking is not possible.**
  /***************************************************************************/
  void set_watch_primitives( const void * var, watchType ref) {
    const watchType * new_ptr = set_watch_primitives_test_ref_val( var);
    set_watch( *new_ptr, ref);
  };
  void set_watch_primitives( const void * var, const void * ref) {
    const watchType * new_ptr = set_watch_primitives_test_ref_var( var,ref);
    const watchType * new_ref = reinterpret_cast<const watchType *>(ref);
    set_watch( *new_ptr, new_ref);
  };
  const watchType * set_watch_primitives_test_ref_val( const void * var)
  {
    if (var == NULL) {
      CMLMessage::fail(
      __FILE__,__LINE__,"Critical failure in calling set_watch\n",
      "Trick was unable to find the specified watch variable.\n"
      "The pointer passed into the set_watch_primitives method is NULL.\n"
      "Check the variable for the correct name.\n");
    }
    CMLMessage::warn(
      __FILE__,__LINE__,"Unsafe method call set_watch_primitives\n",
      "The void pointer(s) returned by 'get_address' and passed into "
      "set_watch_primitives\nis about to be reinterpret_cast to the type "
      "associated with the event.\n"
      "If the specified variable is not of the correct type, subsequent "
      "behavior will be undefined. \n"
      "Double check the variable type.\n");
    return reinterpret_cast<const watchType *>(var);
  };
  const watchType * set_watch_primitives_test_ref_var( const void * var, const void * ref)
  {
    if (var == NULL && ref != NULL) {
      CMLMessage::fail(
      __FILE__,__LINE__,"Critical failure in calling set_watch\n",
      "Trick was unable to find the specified watch variable.\n"
      "The pointer passed into the set_watch_primitives method is NULL.\n"
      "Check the variable for the correct name.\n");
    }
    else if (var != NULL && ref == NULL) {
      CMLMessage::fail(
      __FILE__,__LINE__,"Critical failure in calling set_watch\n",
      "Trick was unable to find the specified reference variable.\n"
      "The pointer passed into the set_watch_primitives method is NULL.\n"
      "Check the variable for the correct name.\n");
    }
    else if (var == NULL && ref == NULL) {
      CMLMessage::fail(
      __FILE__,__LINE__,"Critical failure in calling set_watch\n",
      "Trick was unable to find the specified watch and reference variables.\n"
      "The pointers passed into the set_watch_primitives method are NULL.\n"
      "Check the variables for the correct names.\n");
    }
    CMLMessage::warn(
      __FILE__,__LINE__,"Unsafe method call set_watch_primitives\n",
      "The void pointer(s) returned by 'get_address' and passed into "
      "set_watch_primitives\nis about to be reinterpret_cast to the type "
      "associated with the event.\n"
      "If the specified variable is not of the correct type, subsequent "
      "behavior will be undefined. \n"
      "Double check the variable type.\n");
    return reinterpret_cast<const watchType *>(var);
  };


/*****************************************************************************
set_reference
Purpose:(Sets the reference value.  Used for changing default or previously
         defined values.)
*****************************************************************************/
  void set_reference(watchType ref) {
    reference = ref;
    return;
  };

 void set_dbl_reference(double ref) override {
    reference = static_cast<watchType>(ref);
  }

/*****************************************************************************
test_crossing
Purpose:(tests for crossing of reference)
Returns: An indicator of whether there has been a change to the vehicle-status
NOTE - "virtual" ends here.  Classes deriving from here should not override
       this method.  The method is virtual in WatchValuesBaseCore because it
       has to be there to be accessed through the WatchValuesBaseCore lists,
       but it is pure-virtual there.  This is the only true implementation of
       this method
******************************************************************************/
  virtual bool test_crossing() override {
    // Sanity check, although this can only be in the array if it is active.
    if (!active) {
      return false;
    }
    event_triggered = false;

    // De-reference the "reference" if using a variable "reference".
    if (reference_is_variable) {
      if (relative_to_activation_protected) {
        set_reference_relative_to_activation(variable);
      }
      else {
        reference = *reference_ptr;
      }
    }
    // Split here for doubles and floats, which test < / >,
    // versus bools, ints, enums, which usually test ==.
    test_crossing(variable);

    // set the active flag in the case that a crossing was detected.
    if (event_triggered) {
      active = multi_shot;
    }

    // Generate the integer version for interfacing with pre-99 C code that can't
    // use bools.
    int_event_triggered = event_triggered ? 1 : 0;
    return event_triggered;
  };


 protected:
/*****************************************************************************
test_crossing
Purpose:(Split behavior based on whether the template class variable type
         is continuous or discrete.)
*****************************************************************************/
  //         Define discrete-variable behavior
  /***************************************************************************/
  template<typename varType>
  void test_crossing( const varType*)
  {
    // Check for treating discrete variable as a continuous variable:
    if (use_threshold_crossing_trigger) {
      test_crossing_dbl( static_cast<double>(*variable),
                         static_cast<double>(reference));
    }

    else {  // default
      event_triggered = (*variable == reference);
    }
  };
  /***************************************************************************/
  //         Define continuous-variable behavior
  /***************************************************************************/
  void test_crossing( const float *)
  {
    test_crossing_dbl( *variable, reference);
  }
  /***************************************************************************/
   void test_crossing(const double *)
  {
    test_crossing_dbl( *variable, reference);
  }
  //***************************************************************************
   virtual void test_crossing_dbl( double var_val,
                                   double ref_val)
  {
    // NOTE - using < and >, not <= and >=.  That requires CROSSING the
    // reference value.
    // This is important, expecially on time-triggers.
    // ref = 0 will trigger NEXT step.
    // ref < 0 will trigger NOW.
    if (direction == Decreasing) {
      event_triggered = (var_val < ref_val);
    }
    else if (direction == Increasing) {
      event_triggered = (var_val > ref_val);
    }
    else if (direction == Both) { // Either direction
      double this_delta =  var_val - ref_val;
      event_triggered = (this_delta * delta_record < 0);

      if (std::abs(this_delta) > 0.0) {
        delta_record = this_delta;
      }
    }
    else { // Undefined
      // Impossible to hit this error because direction is protected,
      // and because set_direction() converts all positive numbers
      // to 1 and all negative numbers to -1
      CMLMessage::error (
      __FILE__, __LINE__, "Invalid Event/Watch activation: No direction:\n",
      "An attempt was made to activate a WatchValue instance with no direction\n"
      "It is unclear whether the event should trigger when the variable is\n"
      "larger or smaller than the reference.\n  Deactivating the watch.\n");
      deactivate();
    }
    return;
  };


/*****************************************************************************
activate
Purpose:(Completes the activation sequence, including verification that the
         appropriate data and pointers have been set in a compatible manner.)
Assumption: (Both activate() and initialize() must have been called)
 ******************************************************************************/
  virtual void activate() {
    if (variable == NULL) {
      CMLMessage::error (
      __FILE__, __LINE__, "Invalid Event/Watch activation: NULL variable:\n",
      "An attempt was made to activate a WatchValue instance with a NULL\n"
      "variable to watch.\n  Attempt failed.\n");
      return;
    }

    if (relative_to_activation) {
      relative_to_activation_protected = true;
      variable_at_activation = *variable;
      set_reference_relative_to_activation( variable);
    }
    active = true;
    if (add_self_to_manager_active_list) {
      if (!active_watches) {
        CMLMessage::error( __FILE__,__LINE__,
          "Activating watch with internal instruction to add itself to\n"
          "the active-watch list, but has no access to that list.\n"
          "Watch will not get checked by the event-manager.\n");
      } else {
        active_watches->push_back(this);
      }
    }
    return;
  };

/*****************************************************************************
set_reference_relative_to_activation
Purpose:(Internal activation sub-method for case where reference is relative\
         to variable-value at activation.
         Process is slightly different if data type is bool.)
*****************************************************************************/
  // ** bool type **
  // *************************************************************************
  void set_reference_relative_to_activation(const bool*)
  {
    if (reference == variable_at_activation) {
        CMLMessage::warn(
          __FILE__,__LINE__,"Possible unintended behavior with bool event\n",
          "An event based on a boolean variable has relative_to_activation\n "
          "set to True, but the value of the watch variable at activation\n "
          "is equal to the reference value. The reference value will be\n "
          "flipped so that the event will trigger when the watch variable\n "
          "flips value relative to its value at activation.\n");
    }
    reference = !variable_at_activation;
    return;
  };
  /***************************************************************************/
  // int
  /***************************************************************************/
  void set_reference_relative_to_activation(const int*)
  { increment_reference();}
  /***************************************************************************/
  // double, same behavior as int
  /***************************************************************************/
  void set_reference_relative_to_activation(const double*)
  { increment_reference();}
  /***************************************************************************/
  // float, same behavior as int
  /***************************************************************************/
  void set_reference_relative_to_activation(const float*)
  { increment_reference();}
  /***************************************************************************/
  // all other data types
  /***************************************************************************/
  template<typename varType>
  void set_reference_relative_to_activation(varType*)
  {
    CMLMessage::error (
      __FILE__, __LINE__, "Invalid Event/Watch activation: undefined behavior:\n",
      "A watch was activated with the flag to watch for a value to be\n"
      "determined relative to the current value.\n"
      "No method has been provided to modify the current value.\n"
      "Using the originally specified reference value instead.\n");
      relative_to_activation = false;
    return;
  };

/*****************************************************************************
increment_reference
Purpose:(Generates the new reference value.)
*****************************************************************************/
  void increment_reference()
  {
    // Evaluate the intended offset if necessary
    if (reference_is_variable) {
      reference = * reference_ptr;
    }
    // Increment the current value with the intended offset.
    reference += variable_at_activation;
    return;
  };

 private:
  WatchValuesBase (const WatchValuesBase& rhs);
  WatchValuesBase& operator = (const WatchValuesBase& rhs);
};
#endif
