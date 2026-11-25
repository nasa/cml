/*******************************************************************************
Purpose:
  (A dummy model that executes messages using the CML message handler)

Programmers:
  (((Gary Turner) (OSR) (March 2023) (ANTARES) (Initial version))
   ((Nino Tarantino) (CACI) (April 2026) (CML) (Refactor for unit testing and mocking))
  )
*******************************************************************************/
#ifndef CML_SAMPLE_MODEL_HH
#define CML_SAMPLE_MODEL_HH
#include <string>

#include "cml/models/utilities/cml_message/include/cml_message.hh"
struct SampleModel {
  bool terminate {false}; /* (--) If true, will terminate the sim on the next update() call */

  void update()
  {
      int ii = 4;
      std::string part2 = "is a collection of ";

      cml::message::publish(
          cml::message::Error,
          __FILE__, __LINE__,
          "This ",part2, ii, " arguments");

      cml::message::publish(
          cml::message::Warning,
          __FILE__, __LINE__,
          "This ",part2, ii," arguments");

      cml::message::status(
          "This ",part2, ii," arguments");

      cml::message::publish(
          cml::message::Inform,
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      cml::message::publish(
          cml::message::Debug,
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      if (terminate) {
        cml::message::fail(
            __FILE__, __LINE__,
            "This is ","a ","terminal error");
      }
    }
};
#endif
