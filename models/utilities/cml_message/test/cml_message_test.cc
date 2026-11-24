#include "../include/cml_message.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace {

// Test that the message publishing level is set properly.
TEST(Message, SetPublishLevel) {
    const std::array publish_levels {
        cml::message::Fail,
        cml::message::Error,
        cml::message::Warning,
        cml::message::Status,
        cml::message::Inform,
        cml::message::Debug
    };

    for (auto level : publish_levels) {
        cml::message::set_publish_level(level);
        EXPECT_EQ(cml::message::get_publish_level(), level);
    }
}

// Test the helper function that combines a variable number of arguments into a single string.
TEST(Message, CollateArgs) {
    const std::string arg4 = "test function";
    auto result = cml::message::collate_args("We supply ", 4, " arguments to ", arg4);
    EXPECT_EQ(result, "We supply 4 arguments to test function");
}

// Test that calling each explicit publishing level function forwards arguments to
// the backend publish() function as expected.
TEST(Message, InterfaceMethods) {
    using testing::_;

    cml::message::Mock cml_message_mock;

    const int ii = 4;
    const std::string part2 = "is a collection of ";
    const char* part3 = " arguments";
    const std::string expected = "This is a collection of 4 arguments";

    cml::message::set_publish_level(cml::message::Debug);

    EXPECT_CALL(cml_message_mock, publish(cml::message::Error, _, _, expected));
    cml::message::error(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::message::Warning, _, _, expected));
    cml::message::warn(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::message::Inform, _, _, expected));
    cml::message::inform(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::message::Debug, _, _, expected));
    cml::message::debug(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::message::Status, _, _, expected));
    cml::message::status("This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::message::Fail, _, _, expected));
    cml::message::fail(__FILE__, __LINE__, "This ", part2, ii, part3);
}

// Test the printf formatting functionality by formatting a floating point value.
TEST(Message, PrintFormat) {
    using testing::HasSubstr;

    const int ii = 4;
    const double x = 123456789.012345;
    const std::string part2 = "is a collection of ";

    // Test a valid format.
    const auto result = cml::message::printf_fmt(
        "%12.6g  %14.12g  %18.12E  %15.3f   %6d",
           x,      x,       x,       x,      ii);
    const std::string expected =
        " 1.23457e+08   123456789.012  1.234567890123E+08    123456789.012        4";
    EXPECT_EQ(result, expected);

    // Test an invalid format.
    EXPECT_THAT(cml::message::printf_fmt("%Z", "bad format specifier"), HasSubstr("ERROR: invalid printf"));
}

}
