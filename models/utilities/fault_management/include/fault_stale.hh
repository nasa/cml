/*############################################################################
PURPOSE:
  (Manages injection of a stale-value fault.)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_STALE_HH
#define CML_FAULT_STALE_HH

#include "fault.hh"

/*******************************************************************************
FaultStale
Purpose:(Manages injection of a stale-value fault.)
*******************************************************************************/
template<typename T> class FaultStale : public Fault {
  public :
    explicit FaultStale(T& var);
    virtual ~FaultStale(){}

    void overwrite_value() override;

    void reset() override { stale_value = variable;
                            Fault::reset();}

    T stale_value; /* (--)
      The value to which the variable is set when it's faulted. */

  private:
    T& variable; /* (--) The variable to fault. */

    FaultStale(const FaultStale&);
    FaultStale& operator = (const FaultStale&);
};


template<typename T>
FaultStale<T>::FaultStale(T& var) :
  variable(var)
{}


/*******************************************************************************
overwrite_value
Purpose:(Injects the fault.)
*******************************************************************************/
template<typename T> void FaultStale<T>::overwrite_value() {
  variable = stale_value;
}

#endif
