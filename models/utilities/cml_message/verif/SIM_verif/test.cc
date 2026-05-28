#include "../../include/cml_message.hh"

//CMLMessage::PublishLevel CMLMessage::publish_level = CMLMessage::Warning;
//const std::array< std::string,5>   CMLMessage::message_description =
//{ColorString::yellow( ColorString::red_background("Critical Failure")),
// ColorString::red("Non-critical Error"),
// ColorString::magenta("Anomaly Warning"),
// ColorString::green("Informational Notice"),
// "Debug Point"};

int main()
{
  int ii = 4;
  std::string part2 = "is a collection of ";

  // shoud ignore the Inform and Debug and add a line
  // about not terminating on the Fail
  CMLMessage::publish(
      CMLMessage::Error,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");

  CMLMessage::publish(
      CMLMessage::Warning,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");
  CMLMessage::publish(
      CMLMessage::Inform,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");
  CMLMessage::publish(
      CMLMessage::Debug,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");
  CMLMessage::publish(
      CMLMessage::Fail,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");

  CMLMessage::publish_level = CMLMessage::Debug;
  // Should pick up the Inform and Debug with the message
  // level set tobe more inclusive.
  CMLMessage::publish(
      CMLMessage::Inform,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");
  CMLMessage::publish(
      CMLMessage::Debug,
      __FILE__, __LINE__,
      "This ",part2,ii," arguments");
  return 1;
}
