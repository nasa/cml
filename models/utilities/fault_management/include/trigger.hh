/*############################################################################
PURPOSE:
  (Individual trigger class)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Bingquan Wang) (OSR) (April 2017) (Antares) (Fixed the float-point number
                                                  equality comparison))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_TRIGGER_HH
#define CML_FAULT_TRIGGER_HH

#include <string>
#include <limits> // std::numeric_limits
#include <cstring> // strcmp
#include <cmath> // std::abs(), std::fmod(), std::max()
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh" // check_equal

/*******************************************************************************
TestEqual_Trait
Purpose:(Contains an equality test. This test is different for integers than for
         floating-point numbers; defining them in a template class like this
         allows for the std::numeric_limits::is_integer check in
         Trigger::compare to be done at compile time.)
*******************************************************************************/
template <class T, bool> class TestEqual_Trait;

template <class T>
class TestEqual_Trait<T, true> {
  public:
    static bool test_equal(const T &val1, const T &val2) {
      return val1==val2;
    }
};

template <class T>
class TestEqual_Trait<T, false> {
  public:
    static bool test_equal(const T &val1, const T &val2) {
      return MathUtils::is_near_equal(val1, val2, static_cast<T>(1.0));
    }
};


/*******************************************************************************
TriggerBase
Purpose:
  Abstract base class for fault triggers. The Fault Manager accesses triggers
  via pointers of this type to avoid having to know the specific type.
  All actual implementations are templatized.
*******************************************************************************/
class TriggerBase {
  public:
    enum Operator_enm {
      LT = 1,
      LE,
      EQ,
      GE,
      GT,
      NE,
      Invalid
    };

    static Operator_enm translate_operator(const char* str) {
      if     (strcmp(str, "LT") == 0) {return LT;}
      else if (strcmp(str, "LE") == 0) {return LE;}
      else if (strcmp(str, "EQ") == 0) {return EQ;}
      else if (strcmp(str, "GE") == 0) {return GE;}
      else if (strcmp(str, "GT") == 0) {return GT;}
      else if (strcmp(str, "NE") == 0) {return NE;}
      else {return Invalid;}
    }

    Operator_enm Operator; /* (--)
      The comparison operator (less-than, equal-to, etc.) */

    std::string name; /* (--) The name of this trigger. */

  protected:
    bool trigger_count_limited; /* (--)
      Flag indicating that the trigger-count is limited.*/
    unsigned long trigger_count; /* (--)
      Count of the number of times this Trigger has been queried while
      triggered.*/
    unsigned long trigger_limit; /* (--)
      Maximum value of trigger_count before trigger becomes unavailable.*/

  public:
    TriggerBase()
      :
      Operator(Invalid),
      trigger_count_limited(false),
      trigger_count(0),
      trigger_limit(0)
    {}
    virtual ~TriggerBase() {}

    virtual bool operate() { return compare();}

    bool check_trigger_count() // called only when Trigger is triggered.
    {
      // avoid overflow by stopping the increment once it reaches its target.
      if (trigger_count >= trigger_limit) {
        return false;
      }
      trigger_count++;
      return true;
    }
    void set_trigger_count( unsigned long count_limit)
    {
      trigger_count_limited = true;
      trigger_limit = count_limit;
    }
    void unset_trigger_count()
    {
      trigger_count_limited = false;
    }

    virtual bool compare() = 0;

    virtual void set_value(double)=0;

  private:
    TriggerBase(const TriggerBase&);
    TriggerBase& operator = (const TriggerBase&);
};

/*******************************************************************************
Trigger
Purpose:
  Class-template for representing fault triggers.
  The template argument is the type of the trigger variable.
  Note that this class can be specialized; such a specialization follows,
  defining Trigger using a std::string template-type.
*******************************************************************************/
template <typename T>
class Trigger : public TriggerBase {
  public :

    ////    Constructors and destructors    ////

    explicit Trigger(const T& var) :
      value(),
      variable(var),
      is_periodic(false),
      periodic_length(0.0),
      periodic_period(0.0),
      initial_periodic_value(0.0),
      is_first_trigger(true)
    {}
    virtual ~Trigger() {}

    ////    Operations    ////

    bool compare() override;

    bool operate() override;

    void set_value(double val) override { value = static_cast<T>(val); }

    void set_periodic(T length, T period);

    void unset_periodic() { is_periodic = false; }

    bool get_periodic() {return is_periodic;}

    ////    Attributes    ////

    T value; /* (--) The value to which the trigger variable is compared. */

  private:
    const T& variable; /* (--) The trigger variable. Its value determines when
      the trigger is triggered. */

    bool is_periodic; /* (--)
      If true, after the trigger is triggered for the first time, it will
      be triggered by periodic values of the trigger variable. This makes the
      most sense if the trigger variable is time. */
    T periodic_length; /* (--)
      The length of each period during which a periodic trigger is
      triggered. */
    T periodic_period; /* (--) The length of a periodic trigger's period. */
    T initial_periodic_value; /* (--)
      The value of the trigger variable when a periodic trigger was first
      triggered. */
    bool is_first_trigger; /* (--)
      True until the trigger has been triggered for the first time. */

    Trigger(const Trigger&);
    Trigger& operator = (const Trigger&);
};

/****************************************************************************
Trigger<std::string> :
Purpose:
  Specialization of Trigger when using std::string as the template parameter.
*****************************************************************************/
template<> class Trigger<std::string> : public TriggerBase {
  public:

    ////    Constructors and destructors    ////

    explicit Trigger(const std::string& varString) : variable(varString) {}
    virtual ~Trigger() {}

    ////    Operations    ////

    bool compare() override {
      bool success = false;

      switch (Operator) {
        case EQ:
          success = value.compare(variable) == 0;
          break;
        case NE:
          success = value.compare(variable) != 0;
          break;
        default: // "Less than" and "greater than" don't make sense for strings.
          success = false;
      }
      return success;
    }

    ////    Attributes    ////

    std::string value; /* (--)
      The value to which the trigger string is compared. */

    void set_value( double) override
      {
        CMLMessage::error(
          __FILE__, __LINE__, "Fault Management Error in Trigger configuration.\n",
          "Arithmetic value assigned to trigger-value in Trigger <", name, ">.\n"
          "Cannot process an arithmetic value into a string.\n"
          "Assignment failed.\n");
      }
    void set_value( std::string val) { value.assign(val);}
  private:
    const std::string& variable; /* (--)
      The trigger string. Its value determines when the trigger is triggered. */

    Trigger(const Trigger&);
    Trigger& operator = (const Trigger&);
};
/****************************************************************************/
template<> class Trigger<bool> : public TriggerBase {
  public:
    explicit Trigger(const bool& var) : variable(var) {}
    virtual ~Trigger() {}

    bool compare() override { return (value == variable); }
    bool value; /* (--)
      The value to which the trigger variable is compared. */

    void set_value( double val) override
    {
      CMLMessage::warn(
        __FILE__, __LINE__,
        "Fault Management Error in Trigger configuration.\n",
        "Arithmetic value assigned to trigger-value in Trigger \"%s\".\n"
        "An arithmetic value is assigned to a bool by the rule:\n"
        ">0 : true\n"
        "<=0: false\n", name);
      value = (val>0);
    }
    void set_value( bool val) { value = val;}
  private:
    const bool& variable; /* (--)
      The trigger variable. Its value determines when the trigger is
      triggered. */
    Trigger(const Trigger&);
    Trigger& operator = (const Trigger&);
};


/*******************************************************************************
compare
Purpose:(Determine whether the trigger condition has been met.)
*******************************************************************************/
template<typename T> bool Trigger<T>::compare() {
  bool is_triggered = false;

  switch (Operator) {
    case LT:
      is_triggered = variable < value;
      break;
    case LE:
      is_triggered = variable <= value;
      break;
    case EQ:  
      is_triggered = TestEqual_Trait<T, std::numeric_limits<T>::is_integer>::
        test_equal(variable, value);
      break;
    case GE:
      is_triggered = variable >= value;
      break;
    case GT:
      is_triggered = variable > value;
      break;
    case NE:
      is_triggered = !TestEqual_Trait<T, std::numeric_limits<T>::is_integer>::
        test_equal(variable, value);
      break;
    default:
      is_triggered = false;
  }

  return is_triggered;
}


/*******************************************************************************
operate
Purpose:(Determine whether the trigger is triggered.)
*******************************************************************************/
template<typename T> bool Trigger<T>::operate() {

  bool isTriggered = TriggerBase::operate();

  if (isTriggered) {

    // Handle periodic trigger information
    // if we haven't been triggered before, cache off the trigger value
    if (is_first_trigger) {
      is_first_trigger = false;
      if (is_periodic) {
        initial_periodic_value = variable;
      }
    }

    if (is_periodic) {
      // Note: fmod is overloaded by type T, with types double, float, and
      // long-double specifically identified. For these types fmod returns a
      // value of the same type. For all other types, the arguments are cast
      // to double and fmod returns a double.
      isTriggered = (std::abs( std::fmod( variable - initial_periodic_value,
                                          periodic_period))
                     <= periodic_length);
    }
  }

  //
  if (trigger_count_limited) {
    if (!isTriggered) {
      // reset trigger_count
      trigger_count = 0;
    }
    else {
      // trigger-off if count too high
      isTriggered = check_trigger_count();
    }
  }
  return isTriggered;
}


/*******************************************************************************
set_periodic
Purpose:(Makes the trigger periodic.)
*******************************************************************************/
/******************************************************************************/
template<typename T> void Trigger<T>::set_periodic(T length, T period)
{
  if (period <= 0.0) {
    CMLMessage::error(__FILE__, __LINE__,
      "Fault-Management Trigger invalid periodicity setting.\n",
      "Attempted to assign invalid period of ", period, " to trigger <", name, ">.\n"
      "Period cannot be negative.\n");
    is_periodic = false;
    return;
  }
  if (length >= period) {
    CMLMessage::error(__FILE__, __LINE__,
      "Fault-Management Trigger invalid periodicity setting.\n",
      "Attempted to assign invalid length of ", length, " to trigger <", name, ">.\n"
      "Length cannot be greater than period (", period, ").\n");
    is_periodic = false;
    return;
  }
  // If the length is 0 or negative, the trigger will never be triggered.

  is_periodic = true;
  periodic_length = length;
  periodic_period = period;
}
#endif
