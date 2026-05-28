/******************************************************************************
PURPOSE:
 (To be used in conjunction with CommandableActionSet.
  Each instance of CommandableAction represents some command that can be
  generated and submitted to flight software to bring about some resulting
  activity.

  The CommandableActionSet instance provides a set of these CommandableAction
  instances with the intent that each set communicates with FSW on a dedicated
  channel/bus.

  Each CommandableAction can be triggered independently, and once triggered,
  the CommandableActionSet that manages this CommandableAction will provide
  the sim-to-FSW interface with information regarding which command needs to
  be issued to FSW.

  The triggers for each CommandableAction are independent of this model and
  could include, for example, a simulation event, or a trajectory-discrete
  operating within the trajectory model. Whatever the source, the setting of
  the command_now flag, or the calling of force_command() will result in the
  appropriate command details being generated in the CommandableActionSet;
  the sim-to_FSW model can determine what actions it neds to take based on
  this single set of values, rather than having to ping each individual
  potential command.)

Library dependency:
  ((../src/commandable_action.cc)
  )

PROGRAMMERS:
  (
  ((Gary Turner) (OSR) (02/22) (Generic concept developed while
     performing verification on LV-commands model))
  )
******************************************************************************/
#ifndef CML_COMMANDABLE_ACTION_HH
#define CML_COMMANDABLE_ACTION_HH

#include <string>

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "commandable_action_set.hh"

/*****************************************************************************
CommandableAction
Purpose:(
    Represents a specific command being issued form the launch vehicle to FSW.
    Having each command in a separate instance provides a convenient way of
    dividing up the possible LV commands so that not all commands need be
    added for every scenario.
    Each instance maintains:
       - self-identification (of which command the instance represents),
       - the status of the command message
    A set of all possible isntances of this class is managed by the
    CommandableActionSet class.
******************************************************************************/
class CommandableAction
{
 protected:
  // External references:
  CommandableActionSet & manager; /* (--)
            Reference to CommandableActionSet containing this instance*/

 public:
  bool enabled;     /* (--)
    Flag indicating whether to consider issuing a command for this action.
    Default: true*/
  bool command_now; /* (--)
    Flag to indicate that the command associated with this action is to be
    issued now, assuming that the enabled flag is true, Default: false*/
  bool multi_shot; /* (--)
    Flag used to indicate whether this action is commandable multiple
    times. Default: false (single-use) */

  CommandableActionSet::CommandStatus message_status; /* (--)
    status of the command issued for this commandable action.*/

 protected:
  std::string name; /* (--)
    Identifying name of this instance, used for messages.*/
  unsigned int commands_issued; /* (--)
    Count of number of commands of this type sent to FSW. Used for
    debugging.*/
  bool temporary_enabled; /* (--)
    Flag used when enabling a disabled model for the purposes of pushing
    through a command via force_command. If true, the instance will be
    disabled again after successfully issuing the command.*/

 public:
  explicit CommandableAction( CommandableActionSet & manager_);
  CommandableAction( std::string            name_,
                     CommandableActionSet & manager_);
  virtual ~CommandableAction(){};

  void update();
  void force_command();
  void flag_command_processed();
  void enable() {enabled = true; command_now = false;}
  void disable() {enabled = false;}
  std::string & get_name() {return name;}

 private:
  // Not implemented, not supported:
  CommandableAction (const CommandableAction&);
  CommandableAction & operator = (const CommandableAction&);
};
#endif