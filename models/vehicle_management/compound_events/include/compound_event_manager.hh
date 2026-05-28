/*******************************TRICK HEADER******************************
PURPOSE: (Manage the advanced compound-events)

LIBRARY DEPENDENCY:
   ((../src/compound_event_manager.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (July 2023) (ANTARES)
    (Merging concepts found in former grok-events and CML-events))
  )
***********************************************************************/
#ifndef CML_EVENT_MANAGEMENT_COMPOUND_EVENTS_MANAGER_HH
#define CML_EVENT_MANAGEMENT_COMPOUND_EVENTS_MANAGER_HH

#include "cml/models/vehicle_management/events_manager/include/vehicle_events_manager.hh"
#include "cml/models/vehicle_management/events_manager/include/watch_values_base_core.hh"
#include "compound_event.hh"
#include "event_trigger.hh"
#include "trick/memorymanager_c_intf.h" //used for ref_attributes()

class CompoundEventsManager : public VehicleEventsManager
{
 protected:
  const double & time; /* (s)
    Reference to sim dynamic-time. This is used when creating new
    CompoundEvent and EventTrigger instances,
    CompoundEvent instances require a reference to time.
    EventTrigger instances require a reference to any continuous variable;
    if the Event Trigger instances are created by the manager, they will
    get a reference to time as their required reference.*/
  std::list< WatchValuesBaseCore*> allocated_events; /* (--)
    The set of events allocated on-the-fly; this instance of
    CompoundEventsManager is responsible for the memory-management cleanup /
    destruction of these event / watch-value instances.*/
  std::list<WatchValuesBaseCore*>  managed_triggers; /* (--)
    List of all triggers being used, used to unlock triggers.*/
  std::list<WatchValuesBaseCore*>  allocated_triggers; /* (--)
    Set of triggers allocated for use by any trigger-set.*/
  EventTriggerSet allocated_triggers_; /* (--)
    An EventTriggerSet instance used only to create and store new triggers
    when it is desirable to allocate new triggers at the manager-level for
    use in multiple events.  The other capabilities of an EventTriggerSet
    are not used by this instance.*/
 public:
  bool enabled; /* (--)
    Simple enabled flag. The VehicleEventsManager base class was originally
    conceived as being a singleton within a sim, managing all basic events.
    This extension could be utilized as one of several events-managers, with
    each being configured for use during a specific mission phase.
    This flag supports the disabling of undesirable components.
    Default: true.*/

  explicit CompoundEventsManager(const double & time);
  virtual ~CompoundEventsManager();

  void initialize();
  void update();

  void add_event(WatchValuesBaseCore &);
  CompoundEvent& create_event();
  void add_trigger(WatchValuesBaseCore* new_trigger);

  WatchValuesBaseCore * get_trigger(std::string name);
  void set_trigger_dbl_reference(WatchValuesBaseCore & trigger, double ref);
  void set_trigger_dbl_reference(std::string name, double ref);

  // void set_trigger_comparison_logic(WatchValuesBaseCore * trigger, double ref);
  // void set_trigger_comparison_logic(std::string name, double ref);

  void set_trigger_delay_offset(WatchValuesBaseCore & trigger, double ref);
  void set_trigger_delay_offset(std::string name, double ref);

  template <typename T>
  EventTrigger<T>& create_trigger(const T & var,
                                  T         ref,
                                  EventTriggerBase::TriggerCondition comparison,
                                  std::string name = "")
  {
    EventTrigger<T> * new_trigger = new EventTrigger<T>(time);
    allocated_triggers.push_back( new_trigger);
    add_trigger( new_trigger);
    new_trigger->set_watch( var, ref);
    new_trigger->comparison_logic = comparison;
    new_trigger->name = name;
    new_trigger->SubscriptionBase::initialize();
    return *new_trigger;
  }

  template <typename T>
  EventTrigger<T>& create_trigger(const T & var,
                                  const T * ref,
                                  EventTriggerBase::TriggerCondition comparison,
                                  std::string name = "")
  {
    EventTrigger<T> * new_trigger = new EventTrigger<T>(time);
    allocated_triggers.push_back( new_trigger);
    add_trigger( new_trigger);
    new_trigger->set_watch( var, ref);
    new_trigger->comparison_logic = comparison;
    new_trigger->name = name;
    new_trigger->SubscriptionBase::initialize();
    return *new_trigger;
  }

  template <typename T>
  EventTrigger<T>& create_trigger(const std::string var_name,
                                  T ref,
                                  EventTriggerBase::TriggerCondition comparison,
                                  std::string name = "")
  {
    T* var = address_from_name(var_name, ref);
    T& var_ref = *var;
    if (var != nullptr) {
      return create_trigger(var_ref, ref, comparison, name);
    }
    else {
      // Return an empty trigger if the trigger can't be found from the var_name.
      EventTrigger<T> * new_trigger = new EventTrigger<T>(time);
      return *new_trigger;
    }
  }

  template <typename T>
  EventTrigger<T>& create_trigger(const std::string var_name,
                                  T * ref,
                                  EventTriggerBase::TriggerCondition comparison,
                                  std::string name = "")
   {
    T* var = address_from_name(var_name, *ref);
    T& var_ref = *var;
    if (var != nullptr) {
      return create_trigger(var_ref, ref, comparison, name);
    }
    else {
      // Return an empty trigger if the trigger can't be found from the var_name.
      EventTrigger<T> * new_trigger = new EventTrigger<T>(time);
      return *new_trigger;
    }
  }

  template <typename T>
  WatchValuesBase<T>& create_trigger( const T & var,
                                      T         ref,
                                      WatchValuesBaseCore::Direction direction,
                                      std::string name = "")
  {
    WatchValuesBase<T> * new_trigger = new WatchValuesBase<T>();
    allocated_triggers.push_back( new_trigger);
    add_trigger( new_trigger);
    new_trigger->set_watch( var, ref);
    new_trigger->set_direction( direction);
    new_trigger->name = name;
    new_trigger->SubscriptionBase::initialize();
    return *new_trigger;
  }

  template <typename T>
  WatchValuesBase<T>& create_trigger( const T & var,
                                      const T * ref,
                                      WatchValuesBaseCore::Direction direction,
                                      std::string name = "")
  {
    WatchValuesBase<T> * new_trigger = new WatchValuesBase<T>();
    allocated_triggers.push_back( new_trigger);
    add_trigger( new_trigger);
    new_trigger->set_watch( var, ref);
    new_trigger->set_direction( direction);
    new_trigger->name = name;
    new_trigger->SubscriptionBase::initialize();
    return *new_trigger;
  }


  // Provide SWIG with the necessary template so it can see through to the
  // layer above.
  #ifdef SWIG
  %template(create_double_trigger) create_trigger<double>;
  %template(create_int_trigger) create_trigger<int>;
  #endif

 private:
  // execution_follow_up is a pure-virtual method in VehicleEventsManager.
  // It is not needed in this type of events-manager so is defined empty.
  void execution_follow_up(){};


  template <typename T>
  T* address_from_name(const std::string var_name, T ref)
  {
    // Used for create_trigger(const std::string...)
    // Returns a pointer to the variable specified by var_name, or nullptr if existence checks don't pass.
    // Does some type-checking as well for the sake of warnings (which requires the type of ref, but not its value).

    REF2 * var_name_ref = ref_attributes( var_name.c_str());
    if ((var_name_ref          == nullptr) ||
        (var_name_ref->attr    == nullptr) ||
        (var_name_ref->address == nullptr)) {
      CMLMessage::error(__FILE__,__LINE__,
        "Unable to find simulation variable ",var_name,
        ".\n Cannot create a trigger based on this name.\n");

      return nullptr;
    }

    std::string type = var_name_ref->attr->type_name;
    TRICK_TYPE trick_type = var_name_ref->attr->type;
    if (trick_type == TRICK_ENUMERATED) {
      type = "int";
    }
    const std::type_info& ref_type = typeid(ref);
    if ((ref_type==typeid(int) && (trick_type != TRICK_INTEGER && trick_type != TRICK_ENUMERATED)) ||
        // (ref_type==typeid(GROK_PROP_JET_STATE) && (trick_type != TRICK_ENUMERATED)) ||
        (ref_type==typeid(double) && trick_type != TRICK_DOUBLE) ||
        (ref_type==typeid(unsigned int) && trick_type != TRICK_UNSIGNED_INTEGER) ||
        (!(ref_type==typeid(double) ||
          ref_type==typeid(int) ||
          ref_type==typeid(unsigned int))
        && trick_type != TRICK_ENUMERATED)) {
        CMLMessage::warn(__FILE__,__LINE__,
          var_name," has type ", type, " (TRICK_TYPE ", std::to_string(trick_type), ") and ref has type ",ref_type.name()
          // , ".\nThese need to be the same type; otherwise the trigger will be wrong."
          );

          // types unknown to us are likely to be enumerations like GROK_PROP_JET_STATE, but having those here caused Trick-CP failures
          // 3/12/2025 Jonathan Jermstad: Added unsigned int to check
      }

    return reinterpret_cast<T*>(var_name_ref->address);
  }

  // Copy-constructor and operator= not implemented / deleted
  CompoundEventsManager( const CompoundEventsManager&);
  CompoundEventsManager& operator=( const CompoundEventsManager&);
};
#endif
