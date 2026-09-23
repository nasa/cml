#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <cmath>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Floating point comparison tolerances.
inline constexpr double double_tolerance = 1e-12;
inline constexpr float float_tolerance = 1e-4;

// Test the protected division function.
TEST(MathUtils, DivideProtected) {
    using testing::_;
    using testing::HasSubstr;

    // Double-precision values.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Divided by zero or overflow"))).Times(1);
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Fail, _, _, HasSubstr("Divided by zero or overflow"))).Times(1);

        ASSERT_NEAR(MathUtils::divide_protected(15.0, 5.0), 3.0, double_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0, 0.0, 70.0), 70.0, double_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0e-300, 1.0e300, 2.0), 0.0, double_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0e38, 1.0e-38, 35.0), 1e76, double_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0e300, 1.0e-300, 35.0, true), 35.0, double_tolerance);
    }

    // Single-precision values.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Divided by zero or overflow"))).Times(1);
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Fail, _, _, HasSubstr("Divided by zero or overflow"))).Times(1);

        ASSERT_NEAR(MathUtils::divide_protected(15.0f, 5.0f), 3.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0f, 0.0f, 3.14f), 3.14f, float_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0e-38f, 1.0e38f, 2.0f), 0.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::divide_protected(1.0e38f, 1.0e-38f, 5.45678f, true), 5.45678, float_tolerance);
    }
}

// Test the protected arccosine function.
TEST(MathUtils, AcosProtected) {
    ASSERT_NEAR(MathUtils::acos_protected(0.877582561890373), 0.5, double_tolerance);
    ASSERT_NEAR(MathUtils::acos_protected(-1.1), M_PI, double_tolerance);
    ASSERT_NEAR(MathUtils::acos_protected(1.1), 0.0, double_tolerance);

    ASSERT_NEAR(MathUtils::acos_protected(0.877582561890373f), 0.5f, float_tolerance);
    ASSERT_NEAR(MathUtils::acos_protected(-1.1f), static_cast<float>(M_PI), float_tolerance);
    ASSERT_NEAR(MathUtils::acos_protected(1.1f), 0.0f, float_tolerance);
}

// Test the protected arcsine function.
TEST(MathUtils, AsinProtected) {
    ASSERT_NEAR(MathUtils::asin_protected(0.479425538604203), 0.5, double_tolerance);
    ASSERT_NEAR(MathUtils::asin_protected(-1.1), -M_PI_2, double_tolerance);
    ASSERT_NEAR(MathUtils::asin_protected(1.1), M_PI_2, double_tolerance);

    ASSERT_NEAR(MathUtils::asin_protected(0.479425538604203f), 0.5f, float_tolerance);
    ASSERT_NEAR(MathUtils::asin_protected(-1.1f),  static_cast<float>(-M_PI_2), float_tolerance);
    ASSERT_NEAR(MathUtils::asin_protected(1.1f),  static_cast<float>(M_PI_2), float_tolerance);
}

// Test the protected square root function.
TEST(MathUtils, SqrtProtected) {
    using testing::_;
    using testing::HasSubstr;

    CMLMessage::Mock cml_message_mock;

    // Square root of a positive number yields the same result.
    ASSERT_NEAR(MathUtils::sqrt_protected(0.0), 0.0, double_tolerance);
    ASSERT_NEAR(MathUtils::sqrt_protected(4.0), 2.0, double_tolerance);
    ASSERT_NEAR(MathUtils::sqrt_protected(16.0), 4.0, double_tolerance);

    ASSERT_NEAR(MathUtils::sqrt_protected(0.0f), 0.0f, float_tolerance);
    ASSERT_NEAR(MathUtils::sqrt_protected(4.0f), 2.0f, float_tolerance);
    ASSERT_NEAR(MathUtils::sqrt_protected(16.0f), 4.0f, float_tolerance);

    // Square root of a negative number emits an error and returns 0.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("Cannot take square root of a negative value"))).Times(2);
    ASSERT_EQ(MathUtils::sqrt_protected(-5.0), 0.0);
    ASSERT_EQ(MathUtils::sqrt_protected(-5.0f), 0.0f);
}

// Test the protected logarithm function.
TEST(MathUtils, LogProtected) {
    using testing::_;
    using testing::HasSubstr;

    // Double-precision values.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Log of negative number"))).Times(1);
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Fail, _, _, HasSubstr("Log of negative number"))).Times(1);

        ASSERT_NEAR(MathUtils::log_protected(1.0), 0.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(M_E), 1.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(1.0e-308), -709.1962086421661, double_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(1.0e308), 709.1962086421661, double_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(-1.0, 5.0), 5.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(-2.0, 0.0, true), 0.0, double_tolerance);
    }

    // Single-precision values.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Log of negative number"))).Times(1);
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Fail, _, _, HasSubstr("Log of negative number"))).Times(1);

        ASSERT_NEAR(MathUtils::log_protected(1.0f), 0.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(static_cast<float>(M_E)), 1.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(1.0e-38f), -87.49823, float_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(1.0e38f), 87.49823, float_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(-1.0f, 5.0f), 5.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log_protected(-2.0f, 0.0f, true), 0.0f, float_tolerance);
    }
}

// Test the protected base-10 logarithm function.
TEST(MathUtils, Log10Protected) {
    using testing::_;
    using testing::HasSubstr;

    // Double precision values.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Log of negative number"))).Times(1);
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Fail, _, _, HasSubstr("Log of negative number"))).Times(1);

        ASSERT_NEAR(MathUtils::log10_protected(1.0), 0.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(10.0), 1.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(1.0e-308), -308.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(1.0e308), 308.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(-4.0, 10.0), 10.0, double_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(0.0, 0.0, true), 0.0, double_tolerance);
    }

    // Single precision values.
    {
        ASSERT_NEAR(MathUtils::log10_protected(1.0f), 0.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(10.0f), 1.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(1.0e-38f), -38.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(1.0e38f), 38.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(-4.0f, 10.0f), 10.0f, float_tolerance);
        ASSERT_NEAR(MathUtils::log10_protected(0.0f, 0.0f, true), 0.0f, float_tolerance);
    }
}

}
