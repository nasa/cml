/*******************************************************************************

Purpose:
  (An updated message-handler system; largely based on defaults developed
  in JEOD.)

Programmers:
  (((Gary Turner) (OSR) (March 2023) (ANTARES) (Initial version))
  )
*******************************************************************************/
#include "../include/cml_message.hh"


CMLMessage::PublishLevel CMLMessage::publish_level = CMLMessage::Warning;
