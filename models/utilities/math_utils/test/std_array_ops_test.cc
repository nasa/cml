#include "../include/math_utils.hh"
#include "../include/std_array_ops.hh"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>

namespace {

// Floating point comparison tolerance.
constexpr double tolerance = 1e-12;

// Test the is_near_equal function for arrays.
TEST(StdArrayOps, Equality) {
    // Double precision
    {
        const std::array<double, 3> lhs {1.0, -2.0, 3.0};
        const std::array<double, 3> rhs1 {1.0, -2.0, 3.0};
        const std::array<double, 3> rhs2 {1.0, 0.0, 0.0};
        EXPECT_TRUE(MathUtils::is_near_equal(lhs, rhs1));
        EXPECT_FALSE(MathUtils::is_near_equal(lhs, rhs2));
    }

    // Single precision
    {
        const std::array<float, 3> lhs {1.0, -2.0, 3.0};
        const std::array<float, 3> rhs1 {1.0, -2.0, 3.0};
        const std::array<float, 3> rhs2 {1.0, 0.0, 0.0};
        EXPECT_TRUE(MathUtils::is_near_equal(lhs, rhs1));
        EXPECT_FALSE(MathUtils::is_near_equal(lhs, rhs2));
    }
}

// Test the std::ostream operator<< overload.
TEST(StdArrayOps, Ostream) {
    const std::array<double, 4> array {1.0, 2.0, 3.3, -4.1234};
    std::stringstream stream;
    stream << array;
    EXPECT_EQ(stream.str(), "[1, 2, 3.3, -4.1234]");
}

// Test the function which copies a C-style array into an std::array.
TEST(StdArrayOps, ConvertFromCStyleArray) {
    using testing::Eq;
    using testing::Pointwise;

    const double vec1[3] {3.1, 3.2, 3.3};
    const double vec2[4] {4.1, 4.2, 4.3, 4.4};
    const std::array<double, 3> vec1_copy = std_copy(vec1);
    const std::array<double, 4> vec2_copy = std_copy(vec2);
    EXPECT_THAT(vec1_copy, Pointwise(Eq(), vec1));
    EXPECT_THAT(vec2_copy, Pointwise(Eq(), vec2));
}

// Test the vector addition functions.
TEST(StdArrayOps, Addition) {
    using testing::DoubleNear;
    using testing::Pointwise;

    // In-place addition.
    {
        std::array<double, 3> lhs {1.0, 2.0, -3.0};

        const double c_style_other[3] {-4.0, 0.0, 1.0};
        const std::array<double, 3> array_other {0.0, 5.0, 10.0};

        lhs += c_style_other;
        EXPECT_THAT(lhs, Pointwise(DoubleNear(tolerance), {-3.0, 2.0, -2.0}));

        lhs += array_other;
        EXPECT_THAT(lhs, Pointwise(DoubleNear(tolerance), {-3.0, 7.0, 8.0}));
    }

    // Addition
    {
        const std::array<double, 3> array1 {2.0, 4.0, 6.0};
        const std::array<double, 3> array2 {-5.0, -10.0, 0.0};
        const double c_array[3] {1.5, -2.5, 5.5};

        EXPECT_THAT(array1 + array2, Pointwise(DoubleNear(tolerance), {-3.0, -6.0, 6.0}));
        EXPECT_THAT(array1 + c_array, Pointwise(DoubleNear(tolerance), {3.5, 1.5, 11.5}));
        EXPECT_THAT(c_array + array2, Pointwise(DoubleNear(tolerance), {-3.5, -12.5, 5.5}));
    }
}

// Test unary operations.
TEST(StdArrayOps, UnaryOperations) {
    using testing::Eq;
    using testing::Pointwise;

    const std::array<double, 3> arr {1.0, -2.0, 3.0};
    EXPECT_THAT(-arr, Pointwise(Eq(), {-1.0, 2.0, -3.0}));
}

// Test the vector subtraction functions.
TEST(StdArrayOps, Subtraction) {
    using testing::DoubleNear;
    using testing::Pointwise;

    // In-place subtraction.
    {
        std::array<double, 3> lhs {1.0, 2.0, -3.0};

        const double c_style_other[3] {-4.0, 0.0, 1.0};
        const std::array<double, 3> array_other {0.0, 5.0, 10.0};

        lhs -= c_style_other;
        EXPECT_THAT(lhs, Pointwise(DoubleNear(tolerance), {5.0, 2.0, -4.0}));

        lhs -= array_other;
        EXPECT_THAT(lhs, Pointwise(DoubleNear(tolerance), {5.0, -3.0, -14.0}));
    }

    // Subtraction
    {
        const std::array<double, 3> array1 {2.0, 4.0, 6.0};
        const std::array<double, 3> array2 {-5.0, -10.0, 0.0};
        const double c_array[3] {1.5, -2.5, 5.5};

        EXPECT_THAT(array1 - array2, Pointwise(DoubleNear(tolerance), {7.0, 14.0, 6.0}));
        EXPECT_THAT(array1 - c_array, Pointwise(DoubleNear(tolerance), {0.5, 6.5, 0.5}));
        EXPECT_THAT(c_array - array2, Pointwise(DoubleNear(tolerance), {6.5, 7.5, 5.5}));
    }
}

// Test the vector multiplication functions.
TEST(StdArrayOps, Multiplication) {
    using testing::DoubleNear;
    using testing::Pointwise;

    // In-place multiplication.
    {
        std::array<double, 3> arr {1.0, -5.0, 4.0};
        const double c_array[3] {0.0, 4.0, -2.5};
        const double scalar = -3.0;

        arr *= scalar;
        EXPECT_THAT(arr, Pointwise(DoubleNear(tolerance), {-3.0, 15.0, -12.0}));

        arr *= c_array;
        EXPECT_THAT(arr, Pointwise(DoubleNear(tolerance), {0.0, 60.0, 30.0}));

        arr *= arr;
        EXPECT_THAT(arr, Pointwise(DoubleNear(tolerance), {0.0, 3600.0, 900.0}));
    }

    // Multiplication.
    {
        const std::array<double, 3> arr {1.0, -5.0, 4.0};
        const double c_array[3] {0.0, 4.0, -2.5};
        const double scalar = -3.0;

        EXPECT_THAT(arr * scalar, Pointwise(DoubleNear(tolerance), {-3.0, 15.0, -12.0}));
        EXPECT_THAT(scalar * arr, Pointwise(DoubleNear(tolerance), {-3.0, 15.0, -12.0}));

        EXPECT_THAT(arr * c_array, Pointwise(DoubleNear(tolerance), {0.0, -20.0, -10.0}));
        EXPECT_THAT(c_array * arr, Pointwise(DoubleNear(tolerance), {0.0, -20.0, -10.0}));

        EXPECT_THAT(arr * arr, Pointwise(DoubleNear(tolerance), {1.0, 25.0, 16.0}));
    }
}

// Test the vector division functions.
TEST(StdArrayOps, Division) {
    using testing::DoubleNear;
    using testing::Pointwise;

    // In-place Division.
    {
        std::array<double, 3> arr {100.0, -500.0, 400.0};
        const double c_array[3] {25.0, 1.0, -10.0};
        const double scalar = -2.0;
        const std::array<double, 3> other {4.0, 5.0, 40.0};

        arr /= scalar;
        EXPECT_THAT(arr, Pointwise(DoubleNear(tolerance), {-50.0, 250.0, -200.0}));

        arr /= c_array;
        EXPECT_THAT(arr, Pointwise(DoubleNear(tolerance), {-2.0, 250.0, 20.0}));

        arr /= other;
        EXPECT_THAT(arr, Pointwise(DoubleNear(tolerance), {-0.5, 50.0, 0.5}));
    }

    // Division.
    {
        const std::array<double, 3> arr {16.0, -4.0, 1.0};
        const double c_array[3] {1.0, 10.0, -2.0};
        const double scalar = -4.0;
        const std::array<double, 3> other {32.0, -4.0, 0.5};

        EXPECT_THAT(arr / scalar, Pointwise(DoubleNear(tolerance), {-4.0, 1.0, -0.25}));
        EXPECT_THAT(scalar / arr, Pointwise(DoubleNear(tolerance), {-0.25, 1.0, -4.0}));

        EXPECT_THAT(arr / c_array, Pointwise(DoubleNear(tolerance), {16.0, -0.4, -0.5}));
        EXPECT_THAT(c_array / arr, Pointwise(DoubleNear(tolerance), {0.0625, -2.5, -2.0}));

        EXPECT_THAT(arr / other, Pointwise(DoubleNear(tolerance), {0.5, 1.0, 2.0}));
    }
}

}
