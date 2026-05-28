/*******************************TRICK HEADER******************************
PURPOSE: (Wrap Trick DataRecordGroup functionality into a more convenient
          and easier to use set of functions.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2016) (Antares) (Move from CEV to CML)))
***********************************************************************/

#ifndef CML_TRICK_LOGGING_HH
#define CML_TRICK_LOGGING_HH

#include<list>
#include<cstring> // NULL

#include "trick/DataRecordGroup.hh"
#include "trick/exec_proto.h" // exec_get_sim_time
#include "cml/models/utilities/cml_message/include/cml_message.hh"

class TrickLogging {
  public:
    std::list<Trick::DataRecordGroup *> group_list;

    TrickLogging()
      :
      group_list(0){};

    /***************************************************************************/
    // Add a data record group to the group list
    /***************************************************************************/
    void add_to_list(Trick::DataRecordGroup * add_me) {
      if (add_me == NULL) {
        CMLMessage::error (
          __FILE__, __LINE__, "TrickLogging::SetupError\n",
          "The intended data record group to add is NULL."
          "\nPlease specify a valid data record group.\n");
        return;
      }
      group_list.push_back(add_me);
    };

    /***************************************************************************/
    // Change the log cycle of a specified data record group
    // NOTE
    // - change will not go into effect until the end of the current cycle (see
    //   RUN_group1_set_cycle_offstep)
    // - To put the change into effect immediately, follow this up with a call
    //   to log_now() (see RUN_group1_set_cycle_offstep_with_reset)
    /***************************************************************************/
    void set_cycle(double cycle) {
      for (std::list<Trick::DataRecordGroup *>::iterator ii = group_list.begin();
           ii != group_list.end();
           ++ii) {
        (*ii)->set_cycle(cycle);
      }
    };
    /***************************************************************************/
    void set_cycle( double cycle, const char * name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        group->set_cycle( cycle);
      }
    };
    /***************************************************************************/
    void set_cycle( double cycle, const std::string & name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        group->set_cycle( cycle);
      }
    };

    /***************************************************************************/
    // Change the log cycle of all the data record groups in the list
    // NOTE
    // - change will not go into effect until the end of the current cycle (see
    //   RUN_group1_set_cycle_offstep)
    // - To put the change into effect immediately, follow this up with a call
    //   to log_now() (see RUN_group1_set_cycle_offstep_with_reset)
    /***************************************************************************/

    /***************************************************************************/
    // Fire off all logging for all data record groups at instant the function
    // is called.
    // NOTE
    // -- This will also reset the logging cycle if the cycle was updated between
    //    time steps
    /***************************************************************************/
    void log_now() {
      for (Trick::DataRecordGroup * group:group_list) {
        log_now( *group);
      }
    };
    /***************************************************************************/
    void log_now( const char * name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        log_now( *group);
      }
    };
    /***************************************************************************/
    void log_now( const std::string & name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        log_now( *group);
      }
    };
    /***************************************************************************/
    void log_now (Trick::DataRecordGroup & drg) {
      if (drg.record) {
        drg.data_record(exec_get_sim_time());
      }
      else {
        drg.enable();
        drg.data_record(exec_get_sim_time());
        drg.write_data(true);
        drg.disable();
      }
    }


    /***************************************************************************
    Log everything "now" and reset the next log time to be the next multiple
    of the latest cycle time referenced from t=0.
    e.g.
    - if cycle-time = 1 and this is called at 3.4:
      - create a new log "now" (at t=3.4) if the group is active
      - next-log query will be scheduled to be at the next multiple of 1.0,
        i.e. t=4.0. 
      - subsequent logs will be at +1.0 intervals, i.e. 5.0, 6.0, etc.
      - If the group is active at t=4.0, a new log will be
        created then. Otherwise, it will just move on to t=5.0, ...

    Circumvents having to wait for the previously scheduled 'next-log-time'
    to be reached.

    data_record() will do the resetting of the log cycle, whether or not the
    dr_group is enabled
    ***************************************************************************/

    /***************************************************************************/
    void reset_next_call_time(Trick::DataRecordGroup & drg) {
      drg.data_record(exec_get_sim_time());
    }

    void reset_next_call_time() {
      for (Trick::DataRecordGroup * group:group_list) {
        reset_next_call_time( *group);
      }
    }

    /***************************************************************************/
    // Disable all data record groups
    /***************************************************************************/
    void disable_all() {
      for (Trick::DataRecordGroup * group:group_list) {
        disable(*group);
      }
    };
    /***************************************************************************/
    void disable( const char * name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        disable(*group);
      }
    };
    /***************************************************************************/
    void disable( const std::string & name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        disable(*group);
      }
    };
    /***************************************************************************/
    void disable( Trick::DataRecordGroup & group) {
      group.disable();
      group.write_data(true); // Flushes out the buffer
    };

    /***************************************************************************/
    // Enable all data record groups
    /***************************************************************************/
    void enable_all() {
      for (Trick::DataRecordGroup * group:group_list) {
        group->enable();
      }
    };

    /***************************************************************************/
    // Enable specified data record group
    /***************************************************************************/
    void enable( const char * name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        group->enable();
      }
    };
    /***************************************************************************/
    void enable( const std::string & name) {
      Trick::DataRecordGroup * group = get_group(name);
      if (group) {
        group->enable();
      }
    };

    /***************************************************************************/
    // Returns a pointer to a Trick::DataRecordGroup specified by name
    /***************************************************************************/
    Trick::DataRecordGroup * get_group( const char * name)
    {
      if (name == nullptr) {
        CMLMessage::error ( __FILE__, __LINE__,
          "No name specified for the intended data record group."
          "\nPlease specify a valid data record group name.\n");
        return nullptr;
      }
      for (Trick::DataRecordGroup * group:group_list) {
        if (group->group_name.compare(name) == 0) {
          return group;
        }
      }
      CMLMessage::error( __FILE__,__LINE__,
        "No DataRecordGroup found with name ",name,".\n"
        "Returning nullptr.\n");
      return nullptr;
    };
    /***************************************************************************/
    Trick::DataRecordGroup * get_group( const std::string & name)
    {
      for (Trick::DataRecordGroup * group:group_list) {
        if (group->group_name == name) {
          return group;
        }
      }
      CMLMessage::error( __FILE__,__LINE__,
        "No DataRecordGroup found with name ",name,".\n"
        "Returning nullptr.\n");
      return nullptr;
    };

  private:
    TrickLogging (const TrickLogging&);
    TrickLogging & operator = (const TrickLogging&);
};
#endif
