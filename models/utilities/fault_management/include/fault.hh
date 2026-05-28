/*############################################################################
PURPOSE:
  (Base class for managing fault injection for a single variable.)

LIBRARY DEPENDENCY:
  ((../src/fault.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_HH
#define CML_FAULT_HH

#include <list>
#include <string> // std::string
#include <cstring> // strcmp

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "trigger_group.hh"

/*******************************************************************************
Fault
Purpose:(Base class for managing fault injection for a single variable.)
*******************************************************************************/
class Fault {
friend class FaultManager;
  public :

    Fault();

    virtual ~Fault() {}

    ////    Operations    ////

    virtual void initialize();

    void update();
    void disable() {was_triggered_last_pass = enabled = false;}

    void add_trigger_group(TriggerGroup& trigger_group);

    virtual bool set_param(std::string param_name,
                           double value,
                           bool reset_nominal = false);

  protected:
    bool is_triggered();

    virtual void overwrite_value() = 0;

    virtual void reset(){fire_count = 0;}

    ////    Attributes    ////
  public:
    bool enabled; /* (--) Whether this fault is enabled. */

    std::string name; /* (--) Fault name. */

    bool is_fire_limited; /* (--) Whether this fault is fire-limited. */
    unsigned long fire_limit; /* (--)
      The maximum number of times a fire-limited fault can be triggered. */

  protected:
    bool initialized; /* (--)
      Whether the fault has been successfully initialized. */
    std::list<TriggerGroup*> trigger_groups; /* (--)
      The fault is triggered when one of these trigger groups is triggered. */
    bool was_triggered_last_pass; /* (--)
      True if the fault was triggered last timestep. Used to determine whether
      the fault should be reset. */
    unsigned long fire_count; /* (--)
      The number of times a fire-limited trigger has been triggered. */

  private:
    Fault(const Fault&);
    Fault& operator = (const Fault&);
};

#endif