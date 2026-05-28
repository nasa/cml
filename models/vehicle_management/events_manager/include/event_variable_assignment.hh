/*******************************************************************************
PURPOSE:
   (Provides the mechanism by which variables can be queued for reassignment
    upon the action of an event.

PROGRAMMERS:
   (
    ((Gary Turner) (OSR) (January 2020) (Antares))
   )
 ******************************************************************************/
#ifndef CML_EVENTS_VARIABLE_ASSIGNMENT_HH
#define CML_EVENTS_VARIABLE_ASSIGNMENT_HH

/*****************************************************************************
EventVariableAssignmentBase
Purpoise:(Provides a convenient base class to support assignment operators
and collects instances of EventVariableAssignment and
EventVariableAssignementRef into a single polymorphic set of type VariableAssignmentBase)
*****************************************************************************/
class EventVariableAssignmentBase
{
 public:
  virtual ~EventVariableAssignmentBase(){};
  virtual void make_assignment() = 0;
};


/*****************************************************************************
EventVariableAssignment
Purpose:(Used to set variable to a preset value)
*****************************************************************************/
template<typename T>
class EventVariableAssignment : public EventVariableAssignmentBase
{
 protected:
  T & variable;
  T   value;

 public:
  EventVariableAssignment( T & var,
                           T val)
    :
    variable(var),
    value(val)
  {}
  void make_assignment() override {variable = value;}
};

/*****************************************************************************
EventVariableAssignmentRef
Purpose:(Used to set a specified variable to the value of a target reference
        variable)
*****************************************************************************/
template<typename T>
class EventVariableAssignmentRef : public EventVariableAssignmentBase
{
 protected:
  T & variable;
  const T & value;

 public:
  EventVariableAssignmentRef( T & var,
                              const T & val)
    :
    variable(var),
    value(val)
  {}
  void make_assignment() override {variable = value;}
};
#endif
