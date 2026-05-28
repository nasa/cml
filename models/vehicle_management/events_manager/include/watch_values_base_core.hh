/*******************************************************************************
PURPOSE:
   (Base class of the event-classes, a.k.a. watch-value-* classes)

LIBRARY DEPENDENCIES:
   (../src/watch_values_base_core.cc)

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (October 2014) (Antares))
    ((Bingquan Wang) (OSR) (April, 2017) (Antares) (Fixed the compilation warning
                     of float-point number equality comparison))
    ((Gary Turner) (OSR) (October 2023) (ANTARES)
      (Separated from WatchValuesBase))
   )
 ******************************************************************************/

#ifndef CML_WATCH_VALUES_BASE_CORE_HH
#define CML_WATCH_VALUES_BASE_CORE_HH

#include <list>
#include <string>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "event_variable_assignment.hh"
#include "event_action_base.hh"

//Forward declaration
class VehicleEventsManager;


/*****************************************************************************
WatchValuesBaseCore
Purpose:(The basic, non-type-dependent components of WatchValuesBase.
         Abstract class.
         Primary purpose is so that I can have a lsit of all instances of the
         class WatchValuesBase<*> regardless of the typename.)
*****************************************************************************/
class WatchValuesBaseCore : public SubscriptionBase {

 public:
  enum Direction {
        Undefined = -2,
        Decreasing = -1,
        Both = 0,
        Increasing = 1};
  bool multi_shot; /* (--)
       Fires at every crossing versus first-crossing only (Default: Off)*/
  bool relative_to_activation; /* (--)
    Flag indicating whether the reference value is to be interpreted as being
    relative to the variable-value at the time this instance was activated.
    E.g. when watching for variable increasing by some amount, the reference
    value will be made equal to that amount plus the value of variable
    recorded at activation.
    (Default:Off) */
  bool event_triggered; // (--)  variable matched specified requirement.
  int  int_event_triggered; // (--) output. Integer version of event_triggered.
  std::string message; /* (--)
             Message to send when event is detected and acted upon.*/
  bool add_self_to_manager_active_list; /* (--)
    Flag indicates whether to automatically add this event to the manager's
    monitored list upon event activations.
    Default: true.*/
  std::string name; /* (--)
    Name, primarily for debugging purposes. May be left blank.*/
  bool locked; /* (--)
    Boolean used to lock at each cycle, so each WatchValuesBase cannot be 
    tested more than once per cycle, leading to unintended behavior.*/
  bool externally_managed; /* (--)
    Boolean setting that bool "locked" is set to once trigger is evaluated
    true: lock after evaluation
      (commonly set in CompoundEventsManager::create_trigger())
    false: do not lock after evaluation, can be evaluated multiple times per
      logging cycle (default, but overriden by create_trigger())  */


 protected:
  Direction direction; // (--) trigger at reference crossing in this direction.
  double delta_record; // (--) recorded delta between variable and reference.
  bool relative_to_activation_protected; /* (--)i
    This is a protected copy of the value of the the relative_to_activation
    flag at activation. relative_to_activation is public to allow it to be
    set easily, but it should not be reverted while a WatchValue is being
    tested, so the value is recorded at activation, and this recorded value
    used during routine updates*/

  std::list<WatchValuesBaseCore *> * active_watches; /* --
             pointer to the list in Events Manager.*/

  std::list<bool *> ext_bool_on; /* (--)
             list of exterior models that this action turns on.*/

  std::list<bool *> ext_bool_off; /* (--)
             list of exterior models that this action turns off.*/

  std::list<SubscriptionBase *> subscribe_models; /* (--)
             list of models that this action causes subscriptions to. */

  std::list<SubscriptionBase *> unsubscribe_models; /* (--)
             list of models that this action causes unsubscriptions to. */

  std::list<SubscriptionBase *> disable_models; /* (--)
             list of models that this action causes to become disabled. */

  std::list< EventVariableAssignmentBase * > assignments; /* (--)
             list of variable assignments this action causes.*/

  std::list< EventActionBase * > actions; /* (--)
             list of encapsulated actions this action causes.*/

 public:
  WatchValuesBaseCore();
  virtual ~WatchValuesBaseCore();


  virtual void initialize( std::list<WatchValuesBaseCore *> * active_watch_in);
  void set_direction(int arg);

  void add_to_ext_bool_on( bool & add_me) {ext_bool_on.push_back(&add_me);}
  void add_to_ext_bool_off( bool & add_me) {ext_bool_off.push_back(&add_me);}
  void add_to_subscribe(SubscriptionBase & add_me)
                                       {subscribe_models.push_back(&add_me);}
  void add_to_unsubscribe(SubscriptionBase & add_me)
                                     {unsubscribe_models.push_back(&add_me);}
  void add_to_disable(SubscriptionBase & add_me)
                                     {disable_models.push_back(&add_me);}
  template<typename T> void add_assignment( T& variable, T value)
  {
    assignments.push_back( new EventVariableAssignment<T>(variable,value));
  }
  template<typename T> void add_assignment_ref( T & variable, const T & value)
  {
    assignments.push_back( new EventVariableAssignmentRef<T>(variable,value));
  }
  void add_action( EventActionBase& action) { actions.push_back(&action);}

  virtual void set_dbl_reference(double) {}
  virtual void set_delay_offset(double) {}

  virtual bool test_crossing() = 0;
  virtual bool specific_execution(){ return false;}
  virtual void apply_complementary_changes();

 private:
  WatchValuesBaseCore (const WatchValuesBaseCore& rhs);
  WatchValuesBaseCore& operator = (const WatchValuesBaseCore& rhs);
};
#endif
