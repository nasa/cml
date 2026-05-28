/*******************************************************************************
Purpose:
  (A dummy model that executes messages using the CML message handler)

Programmers:
  (((Gary Turner) (OSR) (March 2023) (ANTARES) (Initial version))
  )
*******************************************************************************/
#ifndef CML_MESSAGE_HANDLER_TEST_HH
#define CML_MESSAGE_HANDLER_TEST_HH
#include <iomanip>

#include "cml/models/utilities/cml_message/include/cml_message.hh"
struct SampleModel {
  bool terminate;

  void update(bool using_aliases)
  {
    int ii = 4;
    double x = 123456789.012345;
    std::string part2 = "is a collection of ";
    if (using_aliases) {
      CMLMessage::error(
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      CMLMessage::warn(
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");
      
      CMLMessage::status(
          "This ",part2, ii," arguments");

      CMLMessage::inform(
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      CMLMessage::debug(
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      CMLMessage::debug(
          __FILE__, __LINE__,
          "Testing printf_fmt: ",
          CMLMessage::printf_fmt("%12.6g  %14.12g  %18.12E  %15.3f   %6d",
                                    x,       x,       x,      x,     ii),
          "\nUnformatted value of x:",x);
          
      CMLMessage::debug(
          __FILE__, __LINE__,
          "Testing printf_fmt error with invalid specifier: ",
          CMLMessage::printf_fmt("%q",ii));

      if (terminate) {
        CMLMessage::fail(
            __FILE__, __LINE__,
            "This is ","a ","terminal error");
      }
    }


    else {
      CMLMessage::publish(
          CMLMessage::Error,
          __FILE__, __LINE__,
          "This ",part2, ii, " arguments");

      CMLMessage::publish(
          CMLMessage::Warning,
          __FILE__, __LINE__,
          "This ",part2, ii," arguments");

      CMLMessage::status(
          "This ",part2, ii," arguments");

      CMLMessage::publish(
          CMLMessage::Inform,
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      CMLMessage::publish(
          CMLMessage::Debug,
          __FILE__, __LINE__,
          "This ",part2,ii," arguments");

      CMLMessage::publish(
          CMLMessage::Debug,
          __FILE__, __LINE__,
          "Testing printf_fmt: ",
          CMLMessage::printf_fmt("%12.6g  %14.12g  %18.12E  %15.3f   %6d",
                                    x,       x,       x,      x,     ii),
          "\nUnformatted value of x:",x);

      CMLMessage::publish(
          CMLMessage::Debug,
          __FILE__, __LINE__,
          "Testing printf_fmt error with invalid specifier: ",
          CMLMessage::printf_fmt("%q",ii));

      if (terminate) {
        CMLMessage::publish(
            CMLMessage::Fail,
            __FILE__, __LINE__,
            "This is ","a ","terminal error");
      }
    }
  }

  SampleModel(){}
  virtual ~SampleModel(){};

 private:
  SampleModel (const SampleModel &);
  SampleModel & operator= (const SampleModel &);

};
#endif
