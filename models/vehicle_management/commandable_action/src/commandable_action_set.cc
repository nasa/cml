/*******************************************************************************
Purpose:
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

Programmers:
  (
  ((Gary Turner) (OSR) (02/22) (Generic concept developed while
     performing verification on LV-commands model))
  )
 (
*******************************************************************************/

#include "../include/commandable_action_set.hh"


/*****************************************************************************
Constructors
*****************************************************************************/
CommandableActionSet::CommandableActionSet ()
  :
  message_status(Inactive),
  name("Unnamed Set"),
  current_commandable(nullptr)
{}
/****************************************************************************/
CommandableActionSet::CommandableActionSet (std::string name_)
  :
  CommandableActionSet()
{
  name= name_;
}

/*****************************************************************************
CommandableActionSet::update
Purpose:
  Main executive
  Runs the update() methods on all commandable actions.
*****************************************************************************/
void
CommandableActionSet::update()
{
  // TODO Turner 05/2018
  //      If the list of available actions gets long, consider creating an
  //      abbreviated list constining only the enabled actions,
  for (CommandableAction* action : commandable_actions) {
    action->update();
  }
}

/*****************************************************************************
enable_all
Purpose:
  enables all commandable-actions to be processed into generating commands.
*****************************************************************************/
void
CommandableActionSet::enable_all()
{
  for (CommandableAction* action : commandable_actions) {
    action->enable();
  }
}

/*****************************************************************************
disable_all
Purpose:
  disables passing of all commands
*****************************************************************************/
void
CommandableActionSet::disable_all()
{
  for (CommandableAction* action : commandable_actions) {
    action->disable();
  }
}

/*****************************************************************************
force_command
Purpose:
  Passthrough / interface method, somewhat redundant because the pass-through
  could be called directly.
*****************************************************************************/
void
CommandableActionSet::force_command(
      CommandableAction & command)
{
  command.force_command();
}

/*****************************************************************************
generate_command
Purpose:
  Called from the commandable-action when its command needs to be generated.
  Returns true if the command has been queued, and false otherwise.
*****************************************************************************/
bool
CommandableActionSet::generate_command (
    CommandableAction & command)
{
  // First check for no other commands are queued.
  // If we have a different command actively queued, drop an error message.
  // Note that this will not affect the status of the incoming commandable,
  // which will keep trying to generate its command until it is informed by
  // this class that its command has been received by the target model.
  // If the current command is already queued, take no further action.
  if (message_status == Send) {
    if (current_commandable != &command &&
        buffered_actions.back() != &command) {
      // NOTE -- current_commandable cannot be NULL at this point because
      // message_status cannot be set to Send unless current_commandable is
      // also set. However, this is an abstract class, intended for extension
      // so there is no guarantee that this pattern will be retained
      // indefinitely.  The following check is currently unreachable.
      if (!current_commandable) {
        CMLMessage::fail(
          __FILE__,__LINE__,"Unknown error\n",
          "current_commandable is NULL but status is Send.\n"
          "This is not a legal combination.\n");
      }

      // and back to the real code:
      std::string & new_name = command.get_name();
      std::string & old_name = current_commandable->get_name();
      CMLMessage::error(
        __FILE__,__LINE__,"Sequencing error\n",
        "[", new_name, "] command ready for issuing from [", name, "]\n"
        "but another command ([", old_name, "]) is already processing.\n"
        "Can only process one command at a time.\n"
        "[", new_name, "] command put on hold.\n");
      buffered_actions.push_back(&command);
    }
    // else no action, already sending this one

    // Return false to the calling instance of CommandableAction -- there is no
    // need to update its command status.
    return false;
  }

  // Make the command and mark it with Send status.
  current_commandable = &command;
  message_status = Send;

  std::string & command_name = command.get_name();
  CMLMessage::inform(
    __FILE__,__LINE__,"Command Issued from\n",
    "[", name, "] issued command [", command_name, "].\n");

  // generate_fsw_command is a pure-virtual method that will support either:
  // - the setting of an enumerated variable according to the specific command
  //   being issued, or:
  // - the direct setting on the FSW bus according to the specific command
  //   being issued.
  // This is not available in this abstract base-class because the
  // identification of the intended set of commands cannot be made in a generic
  // way, this requires a concrete derivative class.
  generate_fsw_command();
  return true;
}

/*****************************************************************************
flag_command_processed
Purpose:
  Sets the status flags in response to postiive feedback from the target model
  that the command has been received and processed.
*****************************************************************************/
void
CommandableActionSet::flag_command_processed()
{
  message_status = Recv;
  if (current_commandable) {
    current_commandable->flag_command_processed();
  }
  // If there is anything buffered, apply the front element now and remove it
  // from the list.
  if (!buffered_actions.empty()) {
    generate_command( *(buffered_actions.front()));
    buffered_actions.pop_front();
  }
}


/*****************************************************************************
generate_fsw_command_safety_net
Purpose:
  Provides a safety net to be called from the derived class in the case that
  none of the implementations match with the specified command.
  Intended usage is in the derived-class generate_fsw_command() method:
    if (command 1) ...
    else if (command 2) ...
    ...
    else generate_fsw_command_safety_net();
*****************************************************************************/
void
CommandableActionSet::generate_fsw_command_safety_net()
{
  if (!current_commandable) {
    // Unreachable code, except by invalid extension.
    CMLMessage::error(
      __FILE__,__LINE__,"NULL command.\n",
      "Current-commandable is NULL which should never be achievable.\n"
      "See CommandableActionSet::generate_command():\n"
      "  current_commandable() is assigned before generate_fsw_command() is "
      "called.\n");
  }
  else {
    // Unreachable code, except by invalid extension.
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid command type.\n",
      "Current-commandable is identified as [", current_commandable->get_name(), "], but this command has no\n"
      "disposition in generate_fsw_commands().\n");
  }
}
