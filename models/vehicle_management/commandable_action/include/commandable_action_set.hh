/******************************************************************************
PURPOSE:
 (Manages commands passing through some channel to the FSW.
  This model provides two classes:
    - CommandableAction represents a specific commandable-action for which a
      command may be issued
    - CommandableActionSet represents the collection of possible commands that
      can be issued on the specific channel.

  This model was written with the intent of providing an interface between some
  triggering mechanism -- such as the trajectory model (trajectory-discretes)
  or the event-manager -- and the interface between the
  sim and the FSW package. The sim-to-FSW interface package is ultimately
  responsible for setting the FSW variable(s) in recognition of a received
  command; this model is responsible for informaing the sim-to-FSW model of
  what commands need setting.
  the
  sim-to-fsw model and either the trajectory model or
  the vehicle-manager events model.  The intent is that either a
  trajectory-discrete or an event-trigger will set the command_now flag, which
  will flag the command as being sent.  Then the sim-to-fsw model will set the
  appropriate FSW flags and mark the command as having been received.

Library dependency:
  ((../src/commandable_action_set.cc)
  )

PROGRAMMERS:
  (
  ((Gary Turner) (OSR) (02/22) (Generic concept developed while
     performing verification on LV-commands model))
  )
******************************************************************************/
#ifndef CML_COMMANDABLE_ACTION_SET_HH
#define CML_COMMANDABLE_ACTION_SET_HH

#include <string>
#include <list>

#include "cml/models/utilities/cml_message/include/cml_message.hh"

// forward declaration
class CommandableAction;

/******************************************************************************
CommandableActionSet
Purpose:(
    Provides the complete set of all possible commandable-actions
    Maintains the status of the communication line and identification of the
    command currently being sent.
******************************************************************************/
class CommandableActionSet
{
 public:
  enum CommandStatus {
    Inactive = 100,  // Inactive - default
    Send     = 101,  // Send - has been created, waiting for send to fsw
    Recv     = 102   // Recv - has been applied to fsw
  };

 protected:
  CommandStatus message_status; /* (--)
          Status associated with active command */

  // TODO Turner Feb 2022
  //      For specific implementation, insert all specific commandable
  //      actions here, for example:
  //   CommandableAction abort;
  //   CommandableAction separate;
  //      and an enumerration of those commands to be used when identifying
  //      what has to be set in response to the current commandable-action.
  //      This should be public:
  //  public:
  //   enum CommandType {
  //     NoOp     = 0,
  //     Abort    = 1,
  //     Separate = 2);
  //  CommandType cmd;

  std::string name; /* (--)
     Name of the set of commands. */
  CommandableAction * current_commandable; /* (--)
    Pointer to the action most recently used for generating a command.*/
  std::list<CommandableAction*> commandable_actions; /* (--)
    Set of pointers to all available commandable actions from which commands
    may be drawn.*/
  std::list<CommandableAction*> buffered_actions; /* (--)
    A buffer of pending commands.*/


 public:
  CommandableActionSet();
  CommandableActionSet(std::string name);
  virtual ~CommandableActionSet(){};

  // main executable:
  void update();

  // user interface
  void enable_all();
  void disable_all();
  void force_command( CommandableAction & command);
  void flag_command_processed();

  // commandable-action interface:
  bool generate_command (CommandableAction & command);
  void add_to_list(CommandableAction & command)
                                      {commandable_actions.push_back(&command);}

  // fsw interface
  CommandStatus get_message_status() {return message_status;}
  bool status_is_send() {return message_status == Send;}
  void generate_fsw_command_safety_net();
 protected:
  virtual void generate_fsw_command() = 0;

 private:
  // Not implemented, not supported:
  CommandableActionSet (const CommandableActionSet&);
  CommandableActionSet & operator = ( const CommandableActionSet&);
};
#include"commandable_action.hh"
#endif