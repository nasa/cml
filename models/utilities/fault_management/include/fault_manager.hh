/*############################################################################
PURPOSE:
  (Fault injection)

LIBRARY DEPENDENCY:
  ((../src/fault_manager.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Refactor for V&V)))
############################################################################*/
#ifndef CML_FAULT_MANAGER_HH
#define CML_FAULT_MANAGER_HH

#include <list>
#include "trick/MemoryManager.hh" // REF2, trick_MM
#include "fault.hh"
#include "fault_bias.hh"
#include "fault_scale.hh"
#include "fault_overwrite.hh"
#include "fault_stale.hh"
#include "fault_function.hh"
#include "fault_white_noise.hh"
#include "fault_random_walk.hh"
#include "trigger.hh"
#include "rand_number.hh"

#include "cml/models/utilities/xml_helper/include/xml_helper.hh" //XmlHelper

#include "cml/models/utilities/cml_message/include/cml_message.hh"

// Forward declaration
class FaultFunctionParameter;

/*******************************************************************************
FaultManager
Purpose:(Constructs and manages a set of faults defined in an XML file.)
*******************************************************************************/
class FaultManager {

  public :
#ifndef SWIG // SWIG doesn't like enum classes
    // Note - this enumeration is cast to an unsigned char in update() and
    // used as an index. If it is edited:
    //   - Location_count must be updated.
    //   - The enumerated values (other than INVALID) must be all the values
    //     between 0 and Location_count-1.
    enum class Location : unsigned char {
      Initialize = 0,
      Upstream = 1,
      Intermediate_1 = 2,
      Intermediate_2 = 3,
      Downstream = 4,
      INVALID = 255
    };
    static const unsigned int Location_count = 5; /* (--)
      The number of possible Locations. INVALID doesn't count. */
    static Location translate_location(const char* str);
    static Location translate_location(const std::string str) {
      return translate_location(str.c_str());
    }
#endif

    ////    Operations    ////

    FaultManager();
    virtual ~FaultManager();
    void initialize();

#ifndef SWIG
    void update(const Location& location);
#endif

    Fault* get_fault(std::string name);
    TriggerBase* get_trigger(std::string name);

    bool set_fault_enabled(std::string fault_name, bool enable_flag);
    bool set_fault_trigger_enabled( std::string fault_name,
                                    std::string trigger_name,
                                    bool enable_flag);

    bool set_fault_param( std::string fault_name,
                          std::string param_name,
                          double value,
                          bool modify_nominal_with_rate = false);

    bool set_trigger_value(std::string trigger_name, double value);
    void unset_trigger_count( std::string trigger_name);

  protected:
    void parse();
    void parse_fault(xmlNodePtr fault_node);

    REF2* get_trick_variable( const char* variable_name);

    template<typename T> Fault* make_fault( xmlNodePtr fault_node,
                                            T& variable,
                                            const char* fault_name);

    template<typename T> Fault* make_fault_bias( xmlNodePtr fault_node,
                                                 T& variable,
                                                 const char* fault_name);

    template<typename T> Fault* make_fault_scale( xmlNodePtr fault_node,
                                                  T& variable,
                                                  const char* fault_name);

    template<typename T> Fault* make_fault_overwrite( xmlNodePtr fault_node,
                                                      T& variable,
                                                      const char* fault_name);

    bool populate_fault_function( FaultFunctionBase* new_fault,
                                  xmlNodePtr fault_node,
                                  const char* fault_name);

    bool parse_ind_variable( FaultFunctionIndependentVariable& ind_variable,
                             xmlNodePtr variable_node,
                             const char* fault_name);


    // the next few functions are virtual to support inserting an interface to
    // another fault-management service (such as fault_arch) that may use
    // other tags for variable names.
    virtual xmlNodePtr check_rand_in_params(xmlNodePtr) {return nullptr;}

    virtual bool parse_non_periodic_param( FaultFunctionParameter& var_param,
                                           xmlNodePtr function_node,
                                           const char* fault_name);

    virtual bool parse_periodic_param( FaultFunctionParameter& var_param,
                                       xmlNodePtr function_node,
                                       const char* param_name,
                                       xmlNodePtr ind_var_node,
                                       const char* fault_name,
                                       bool nom_required);

    virtual bool parse_rand_number( FaultRandNumber& rng,
                                    xmlNodePtr rand_node,
                                    const char* fault_name);

    template<typename T> Fault* make_fault_white_noise( xmlNodePtr fault_node,
                                                        T& variable,
                                                        const char* fault_name);

    template<typename T> Fault* make_fault_random_walk( xmlNodePtr fault_node,
                                                        T& variable,
                                                        const char* fault_name);

    TriggerGroup* parse_trigger_group( xmlNodePtr trigger_group_node,
                                       const char* fault_name);

    TriggerBase* parse_trigger( xmlNodePtr trigger_node,
                                const char* fault_name);

    template<typename T>  TriggerBase* make_trigger( T& variable,
                                                     xmlNodePtr trigger_node,
                                                     const char* value);
    template<typename T> T generate_random_value()
    {
      return rand.get_random_number();
    }

  public:
    std::string fault_file; /* (--) Name of an XML file that defines faults. */

    bool enabled; /* (--)
      If true, this object will parse the XML file and construct faults
      accordingly. If false (default), this object will do nothing. */
    static bool global_enabled; /* (--)
      Set this to false to disable all FaultManagers. */

  private:
    bool parsed; /* (--)
      If true, fault_file is the name of a valid XML fault file and that file has
      been parsed. */

    std::list<Fault*> faults[Location_count]; /* (--)
      Pointers to the individual faults, stored as pointers to the base class to
      support polymorphic storage. The faults themselves are allocated when the
      XML file is parsed. */
    std::list<TriggerBase*> triggers; /* (--)
      Pointers to individual triggers, stored as pointers to the base class to
      support polymorphic storage. The triggers themselves are allocated when
      the XML file is parsed. */
    std::list<TriggerGroup*> trigger_groups; /* (--)
      Pointers to the trigger groups, stored as pointers because the groups are
      non-copyable. The groups themselves are allocated when the XML file is
      parsed. */

    FaultRandNumber rand; /* (--) Random number generator. */

    std::list<std::pair<std::string, bool> > set_enable_for_fault_cache; /* (--)
      Cache for set_fault_enabled commands sent before the XML file is parsed. */

    struct SetTriggerEnableEntry {
      std::string fault_name; /* (--) Fault name. */
      std::string trigger_name; /* (--) Trigger name. */
      bool enable_flag; /* (--) Whether to enable or disable the trigger. */
    }; /* Structure for storing set_fault_trigger_enabled commands. */
    std::list<SetTriggerEnableEntry> set_trigger_enable_for_fault_cache; /* (--)
      Cache for set_fault_trigger_enabled commands sent before the XML file is
      parsed. */

    struct SetFaultParamEntry {
      std::string fault_name; /* (--) Fault name. */
      std::string param_name; /* (--) Parameter name. */
      double value; /* (--) Value to set the parameter to. */
    }; /* Structure for storing set_fault_param() commands. */
    std::list<SetFaultParamEntry> set_fault_param_cache; /* (--)
      Cache for set_fault_param commands sent before the XML file is parsed. */

    std::list<std::pair<std::string, double> > set_trigger_value_cache; /* (--)
      Cache for set_trigger_value commands sent before the XML file is parsed. */

    // Make the class non-copyable
    FaultManager(const FaultManager&);
    FaultManager& operator = (const FaultManager&);
};
template<> bool FaultManager::generate_random_value<bool>();

#endif
