/*******************************************************************************
Purpose:
 (Defines the methods in the CommandableAction class -- see
 commandable_action.hh for details)

Programmers:
 (
  ((Gary Turner) (OSR) (02/22) (Generic concept developed while
     performing verification on LV-commands model))
 )
*******************************************************************************/

#include "../include/commandable_action.hh"


/*****************************************************************************
Constructors
*****************************************************************************/
CommandableAction::CommandableAction (
  CommandableActionSet & manager_)
  :
  manager( manager_),
  enabled(true),
  command_now(false),
  multi_shot(false),
  message_status(CommandableActionSet::Inactive),
  name("Unnamed Command"),
  commands_issued(0),
  temporary_enabled(false)
{
  manager.add_to_list(*this);
}
/****************************************************************************/
CommandableAction::CommandableAction (
  std::string            name_,
  CommandableActionSet & manager_)
  :
  CommandableAction(manager_)
{
  name = name_;
}
/*****************************************************************************
update
Purpose:(Evaluate conditions for sending the command, set if met)
*****************************************************************************/
void
CommandableAction::update()
{
  // return early with no new command if:
  // - the command_now flag has not been tripped OR
  // - this particular commandable-action is disabled.

  if (!command_now ||
      !enabled) {
    return;
  }

  // Attempt to generate the command associated with this action:
  if (manager.generate_command( *this)) {
    message_status = CommandableActionSet::Send;
  }
}

/*****************************************************************************
force_command
Purpose:(Command immediately)
*****************************************************************************/
void
CommandableAction::force_command()
{
  // set message_status to inactive to pick up the informational message when
  // the command is issued.
  message_status = CommandableActionSet::Inactive;

  // Ensure the instance is enabled; this is a necessary step to issue the
  // command. However, if the model is not enabled, set the temporary_enabled
  // flag so the general enabled flag can be reverted upon successful command,
  temporary_enabled = !enabled;
  enabled = true;

  // flag as ready to issue a command and run update() to issue it.
  command_now = true;
  update();
}

/*****************************************************************************
flag_command_processed
Purpose:
  Marks the command as having been received
*****************************************************************************/
void
CommandableAction::flag_command_processed()
{
  commands_issued++;
  // commandable-action is repeatable, reset to support future command
  // generation
  if (multi_shot) {
    message_status = CommandableActionSet::Inactive;
    command_now = false;
    // if the instance was only temporarily enabled, disable it again,
    // returning it to its original state.
    if (temporary_enabled) {
      enabled = false;
    }
  }
  // otherwise, identify this action as complete.
  else {
    message_status = CommandableActionSet::Recv;
    enabled = false;
  }
}
