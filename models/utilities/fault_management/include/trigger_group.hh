/*############################################################################
PURPOSE:
  (Handling for groups of triggers)

LIBRARY DEPENDENCY:
  ((../src/trigger_group.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_TRIGGER_GROUP_HH
#define CML_FAULT_TRIGGER_GROUP_HH

#include "trigger.hh"
#include <list>
#include <string>

/*******************************************************************************
TriggerGroup
Purpose:(A group of fault triggers.)
*******************************************************************************/
class TriggerGroup {
  public :

    TriggerGroup() : enabled(true) {}
    virtual ~TriggerGroup(){}

    ////    Operations    ////

    bool operate();

    void add_trigger(TriggerBase& trigger);

    bool set_trigger_enable(std::string trigger_name, bool enable_flag);

    ////    Attributes    ////

    bool enabled; /* (--) Whether this group is enabled. */

 protected:
   std::list<std::pair<bool, TriggerBase*> > triggers; /* (--)
     List of triggers and whether they are enabled for this group. */

 private:
   // Class is non-copyable
   TriggerGroup(const TriggerGroup&);
   TriggerGroup& operator = (const TriggerGroup&);
};

#endif
