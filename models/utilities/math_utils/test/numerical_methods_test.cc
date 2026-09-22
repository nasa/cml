#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <cmath>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(MathUtils, IsNearEqual) {

}

TEST(MathUtils, IsEqual) {

}

TEST(MathUtils, HasChangedFrom) {

}

TEST(MathUtils, IsWithinRange) {

}

TEST(MathUtils, IsWithinAbsoluteTolerance) {

}

TEST(MathUtils, IsWithinRelativeTolerance) {

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

TEST(MathUtils, Polynomial) {

}

TEST(MathUtils, QuadraticSolver) {

}
