/*******************************TRICK HEADER******************************
PURPOSE: (A sample set for testing commandable actions)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Feb 2022) (Antares) (verification)))
***********************************************************************/
#ifndef SAMPLE_COMMANDABLE_ACTION_SET_HH
#define SAMPLE_COMMANDABLE_ACTION_SET_HH

#include <string>
#include "../../../include/commandable_action_set.hh"
#include "../../../include/commandable_action.hh"

/*****************************************************************************
FswBus
Purpose:
  represents a hypothetical sim-to-FSW data bus, each implementation uses its
  own copy of this bus
*****************************************************************************/
class FswBus
{
 public:
  bool fsw1;
  bool fsw2;
  bool fsw3;
  bool fsw4;

  void reset_values()
  {
     fsw1 = fsw2 = fsw3 = fsw4 = 0;
  }
};

/*****************************************************************************
IndirectCommandableActionSet
Purpose:
  test implementation of CommandableActionSet
*****************************************************************************/
class IndirectCommandableActionSet : public CommandableActionSet
{
 public:
  CommandableAction act1;
  CommandableAction act2;
  CommandableAction act3;
  CommandableAction act4;
  CommandableAction error;

  enum CommandType{
    NoOp = 0,
    ACT1 = 1,
    ACT2 = 2,
    ACT3 = 3,
    ACT4 = 4
  };

  CommandType command_type;

  IndirectCommandableActionSet()
    :
    CommandableActionSet("indirect"),
    act1( "act1", *this),
    act2( *this),
    act3( "act3", *this),
    act4( "act4", *this),
    error("error1", *this),
    command_type( NoOp)
  { }

  virtual ~IndirectCommandableActionSet(){};

  void generate_fsw_command()
  {
    if      (current_commandable == &act1) command_type = ACT1;
    else if (current_commandable == &act2) command_type = ACT2;
    else if (current_commandable == &act3) command_type = ACT3;
    else if (current_commandable == &act4) command_type = ACT4;
    else {
      generate_fsw_command_safety_net();
      command_type = NoOp;
    }
  }
};

/*****************************************************************************
DirectCommandableActionSet
Purpose:
  test implementation of CommandableActionSet
*****************************************************************************/
class DirectCommandableActionSet : public CommandableActionSet
{
 protected:
  FswBus & fsw_bus;
 public:
  CommandableAction act1;
  CommandableAction act2;
  CommandableAction act3;
  CommandableAction act4;
  CommandableAction error;

  DirectCommandableActionSet(FswBus & bus)
    :
    CommandableActionSet(),
    fsw_bus(bus),
    act1( "act1", *this),
    act2( *this),
    act3( "act3", *this),
    act4( "act4", *this),
    error("error2", *this)
  { }

  virtual ~DirectCommandableActionSet(){};

  void generate_fsw_command()
  {
    if      (current_commandable == &act1) fsw_bus.fsw1 = true;
    else if (current_commandable == &act2) fsw_bus.fsw2 = true;
    else if (current_commandable == &act3) fsw_bus.fsw3 = true;
    else if (current_commandable == &act4) fsw_bus.fsw4 = true;
    else generate_fsw_command_safety_net();
    flag_command_processed();
  }
};
#endif
