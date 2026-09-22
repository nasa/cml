#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <cmath>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>

namespace {

// Test the is_near_equal function.
TEST(MathUtils, IsNearEqual) {
    // Double precision values.
    EXPECT_TRUE(MathUtils::is_near_equal(3.45, 3.45));
    EXPECT_TRUE(MathUtils::is_near_equal(1.0000000000000001, 1.0));
    EXPECT_FALSE(MathUtils::is_near_equal(1.000000000000001, 1.0));
    EXPECT_FALSE(MathUtils::is_near_equal(5.4321, 5.4322, 1.0));
    EXPECT_FALSE(MathUtils::is_near_equal(5.4321, 5.4322, 1e7));
    EXPECT_TRUE(MathUtils::is_near_equal(5.4321, 5.4322, 1e15));

    // Single precision values.
    EXPECT_TRUE(MathUtils::is_near_equal(3.45f, 3.45f));
    EXPECT_TRUE(MathUtils::is_near_equal(1.00000001f, 1.0f));
    EXPECT_FALSE(MathUtils::is_near_equal(1.0000001f, 1.0f));
    EXPECT_FALSE(MathUtils::is_near_equal(5.4321f, 5.4322f, 1.0f));
    EXPECT_TRUE(MathUtils::is_near_equal(5.4321f, 5.4322f, 1e7f));
}

// Test the is_equal function.
TEST(MathUtils, IsEqual) {
    EXPECT_TRUE(MathUtils::is_equal(3, 3));
    EXPECT_FALSE(MathUtils::is_equal(3, 4));

    EXPECT_TRUE(MathUtils::is_equal(true, true));
    EXPECT_FALSE(MathUtils::is_equal(true, false));

    EXPECT_TRUE(MathUtils::is_equal(3.45, 3.45));
    EXPECT_FALSE(MathUtils::is_equal(3.45, 3.56));

    EXPECT_TRUE(MathUtils::is_equal(3.45f, 3.45f));
    EXPECT_FALSE(MathUtils::is_equal(3.45f, 3.56f));
}

// Test the has_changed_from function.
TEST(MathUtils, HasChangedFrom) {
    EXPECT_TRUE(MathUtils::has_changed_from(3.45, 4.56));
    EXPECT_TRUE(MathUtils::has_changed_from(4.56, 3.45));
    EXPECT_FALSE(MathUtils::has_changed_from(3.45, 3.45));

    EXPECT_TRUE(MathUtils::has_changed_from(3.45f, 4.56f));
    EXPECT_TRUE(MathUtils::has_changed_from(4.56f, 3.45f));
    EXPECT_FALSE(MathUtils::has_changed_from(3.45f, 3.45f));
}

// Test the absolute tolerance comparison function.
TEST(MathUtils, IsWithinAbsoluteTolerance) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Nominal tests.
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(3.45, 3.46, 0.01));
    EXPECT_FALSE(MathUtils::is_within_abs_tolerance(3.45, 3.46, 0.0099));
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(3, -2, 5));
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(3U, 2U, 2U));
    EXPECT_FALSE(MathUtils::is_within_abs_tolerance(-2.0f, -2.01f, 0.0099f));

    // Boolean cases.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("ambiguous interpretation"))).Times(5);
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(true, true, true));
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(true, true, false));
    EXPECT_FALSE(MathUtils::is_within_abs_tolerance(true, false, false));
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(false, false, true));
    EXPECT_TRUE(MathUtils::is_within_abs_tolerance(false, false, false));

    // A negative absolute tolerance will emit an error.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("not well-defined when the tolerance is negative"))).Times(1);
    EXPECT_FALSE(MathUtils::is_within_abs_tolerance(-2.0f, -2.01f, -1.0f));
}

// Test the relative tolerance comparison function.
TEST(MathUtils, IsWithinRelativeTolerance) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;


    // Nominal tests.
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(1.65, 1.5, 0.1));
    EXPECT_FALSE(MathUtils::is_within_rel_tolerance(1.65, 1.5, 0.09999));
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(2, -2, 2.0));
    EXPECT_FALSE(MathUtils::is_within_rel_tolerance(10U, 2U, 2.0));
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(-1.65f, -1.5f, 0.1f));

    // Boolean tests.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("ambiguous interpretation"))).Times(5);
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(true, true, true));
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(true, true, false));
    EXPECT_FALSE(MathUtils::is_within_rel_tolerance(true, false, false));
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(false, false, true));
    EXPECT_TRUE(MathUtils::is_within_rel_tolerance(false, false, false));

    // A negative relative tolerance will emit an error.
    EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("not well-defined when the tolerance is negative"))).Times(1);
    EXPECT_FALSE(MathUtils::is_within_rel_tolerance(-2.0f, -2.01f, -1.0));
}

// Test the function which returns the sign of the input value.
TEST(MathUtils, Sign) {
    EXPECT_EQ(MathUtils::sign(-5.0), -1);
    EXPECT_EQ(MathUtils::sign(-1.0f), -1);
    EXPECT_EQ(MathUtils::sign(-10), -1);

    EXPECT_EQ(MathUtils::sign(5.0), 1);
    EXPECT_EQ(MathUtils::sign(1.0f), 1);
    EXPECT_EQ(MathUtils::sign(10), 1);

    EXPECT_EQ(MathUtils::sign(0.0), 0);
    EXPECT_EQ(MathUtils::sign(-0.0), 0);
    EXPECT_EQ(MathUtils::sign(-0.0f), 0);
    EXPECT_EQ(MathUtils::sign(0), 0);
}

// Test the polynomial evaluation math.
TEST(MathUtils, Polynomial) {
    using testing::_;
    using testing::HasSubstr;

    // Floating point comparison tolerance.
    constexpr double tolerance = 1e-12;

    // Nominal case: y = 5x^2 + 4x + 3, y(2) = 31
    {
        const std::vector coeffs {3.0, 4.0, 5.0};
        EXPECT_NEAR(MathUtils::polynomial(2.0, coeffs), 31.0, tolerance);
    }

    // Overflow case, use fallback value.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("The polynomial sum is overflow")));

        const std::vector coeffs {3.0, 4.0, 5.0};
        EXPECT_NEAR(MathUtils::polynomial(1e308, coeffs, 75.7575), 75.7575, tolerance);
    }

    // Test evaluation at a very small value.
    {
        const std::vector coeffs {0.0, 4.0, 5.0};
        EXPECT_NEAR(MathUtils::polynomial(-1e-308, coeffs), -4e-308, tolerance);
    }

    // Higher order polynomial.
    {
        const std::vector coeffs {0.0, 4.0, 5.0, -1.0, -1.0};
        EXPECT_NEAR(MathUtils::polynomial(-2.0, coeffs), 4.0, tolerance);
    }

    // Overflow case with failure enabled.
    {
        CMLMessage::Mock cml_message_mock;
        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Fail, _, _, HasSubstr("The polynomial sum is overflow")));

        const std::vector coeffs {5.0, 4.0, 3.0};
        EXPECT_NEAR(MathUtils::polynomial(1e308, coeffs, 0.0, true), 0.0, tolerance);
    }
}

}
