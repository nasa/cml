#include "../include/cml_message.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace {

// Test that the message publishing level is set properly.
TEST(CMLMessage, SetPublishLevel) {
    const std::array publish_levels {
        cml::CMLMessage::Fail,
        cml::CMLMessage::Error,
        cml::CMLMessage::Warning,
        cml::CMLMessage::Status,
        cml::CMLMessage::Inform,
        cml::CMLMessage::Debug
    };

    for (auto level : publish_levels) {
        cml::CMLMessage::set_publish_level(level);
        EXPECT_EQ(cml::CMLMessage::get_publish_level(), level);
    }
}

// Test the helper function that combines a variable number of arguments into a single string.
TEST(CMLMessage, CollateArgs) {
    const std::string arg4 = "test function";
    auto result = cml::CMLMessage::collate_args("We supply ", 4, " arguments to ", arg4);
    EXPECT_EQ(result, "We supply 4 arguments to test function");
}

// Test that calling each explicit publishing level function forwards arguments to
// the backend publish() function as expected.
TEST(CMLMessage, InterfaceMethods) {
    using testing::_;

    cml::CMLMessage::Mock cml_message_mock;

    const int ii = 4;
    const std::string part2 = "is a collection of ";
    const char* part3 = " arguments";
    const std::string expected = "This is a collection of 4 arguments";

    cml::CMLMessage::set_publish_level(cml::CMLMessage::Debug);

    EXPECT_CALL(cml_message_mock, publish(cml::CMLMessage::Error, _, _, expected));
    cml::CMLMessage::error(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::CMLMessage::Warning, _, _, expected));
    cml::CMLMessage::warn(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::CMLMessage::Inform, _, _, expected));
    cml::CMLMessage::inform(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::CMLMessage::Debug, _, _, expected));
    cml::CMLMessage::debug(__FILE__, __LINE__, "This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::CMLMessage::Status, _, _, expected));
    cml::CMLMessage::status("This ", part2, ii, part3);

    EXPECT_CALL(cml_message_mock, publish(cml::CMLMessage::Fail, _, _, expected));
    cml::CMLMessage::fail(__FILE__, __LINE__, "This ", part2, ii, part3);
}

// Test the printf formatting functionality by formatting a floating point value.
TEST(CMLMessage, PrintFormat) {
    using testing::HasSubstr;

    const int ii = 4;
    const double x = 123456789.012345;
    const std::string part2 = "is a collection of ";

    // Test a valid format.
    const auto result = cml::CMLMessage::printf_fmt(
        "%12.6g  %14.12g  %18.12E  %15.3f   %6d",
           x,      x,       x,       x,      ii);
    const std::string expected =
        " 1.23457e+08   123456789.012  1.234567890123E+08    123456789.012        4";
    EXPECT_EQ(result, expected);

    // Test an invalid format.
    EXPECT_THAT(cml::CMLMessage::printf_fmt("%Z", "bad format specifier"), HasSubstr("ERROR: invalid printf"));
}

}
