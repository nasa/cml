/*******************************TRICK HEADER************************************
PURPOSE: (Interface to the newer FaultManagement model to support backward
compatibility with models using the legacy fault_arch sensor-faults.)

LIBRARY DEPENDENCY:
  ((../src/sSensorFaults.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Mar 2022) (Antares) (Initial)))
*******************************************************************************/
#ifndef CML_FAULT_ARCH_SENSORFAULT
#define CML_FAULT_ARCH_SENSORFAULT
#include "cml/models/utilities/fault_management/include/fault_manager.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*******************************************************************************
sSFault
Purpose:
  This is the enumeration pulled from sSFault from the old fault_arch model.
  Legacy uses may be setting locations to this enumeration.
*******************************************************************************/
enum sSFault
{
  INIT           = 0,
  UPSTREAM       = 1,
  INTERMEDIATE_1 = 2,
  INTERMEDIATE_2 = 3,
  DOWNSTREAM     = 4,
  INVALID        = 5
};


/*******************************************************************************
sSensorFaults
Purpose:
  A simple interface so that legacy models using an instance of sSensorFaults
  will instead get access to FaultManager.
*******************************************************************************/
class sSensorFaults : public FaultManager
{
  public:
    sSensorFaults(){};
    virtual ~sSensorFaults(){}

    void Injection( sSFault loc);

    void Init() {initialize();}

  protected:
    bool parse_non_periodic_param( FaultFunctionParameter&  params,
                                   xmlNodePtr               function_node,
                                   const char*              fault_name);
    xmlNodePtr check_rand_in_params(xmlNodePtr  fault_node);

    bool parse_rand_number( FaultRandNumber&  rng,
                            xmlNodePtr        rand_node,
                            const char*       fault_name);

    bool parse_periodic_param( FaultFunctionParameter&  var_param,
                               xmlNodePtr               function_node,
                               const char*              param_name,
                               xmlNodePtr               ind_var_node,
                               const char*              fault_name,
                               bool                     nom_required);

  private:
    // Make the class non-copyable
    sSensorFaults(const sSensorFaults&);
    sSensorFaults& operator = (const sSensorFaults&);
};
#endif
