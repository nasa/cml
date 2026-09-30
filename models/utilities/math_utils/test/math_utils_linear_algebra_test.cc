#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Floating point comparison tolerance.
constexpr double tolerance = 1e-12;

// Test addition and subtraction of C-style vectors.
TEST(MathUtils, VectorAlgebra) {
    using testing::DoubleNear;
    using testing::Pointwise;

    const double lhs[5] {1.0, 2.0, 3.0, 4.0, 5.0};
    const double rhs[5] {-3.0, 6.0, 0.0, 2.0, -1.0};
    const std::array<double, 5> diff = MathUtils::diff(lhs, rhs);
    const std::array<double, 5> sum = MathUtils::sum(lhs, rhs);

    EXPECT_THAT(diff, Pointwise(DoubleNear(tolerance), {4.0, -4.0, 3.0, 2.0, 6.0}));
    EXPECT_THAT(sum, Pointwise(DoubleNear(tolerance), {-2.0, 8.0, 3.0, 6.0, 4.0}));
}

// Test vector magnitude functions.
TEST(MathUtils, VectorMagnitude) {
    // Test the Pythagorean quintuple, [1, 2, 8, 10, 13].
    const std::array<double, 4> arr4 {-1.0, 2.0, -8.0, -10.0};
    const double c_arr4[4] {-1.0, 2.0, -8.0, -10.0};
    EXPECT_NEAR(MathUtils::vec_mag_sq(arr4), 169.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag_sq(c_arr4), 169.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag(arr4), 13.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag(c_arr4), 13.0, tolerance);

    // Magnitude of vector components using the Euler brick [44, 117, 240].
    const std::array<double, 3> arr3 {44.0, 117.0, 240.0};
    const double c_arr3[3] {44.0, 117.0, 240.0};
    EXPECT_NEAR(MathUtils::vec_mag_xy(arr3), 125.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag_xy(c_arr3), 125.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag_xz(arr3), 244.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag_xz(c_arr3), 244.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag_yz(arr3), 267.0, tolerance);
    EXPECT_NEAR(MathUtils::vec_mag_yz(c_arr3), 267.0, tolerance);
}

// Test unit vector functions.
TEST(MathUtils, UnitVector) {
    using testing::DoubleNear;
    using testing::Eq;
    using testing::Pointwise;

    // Unit vector calculation. Magnitude is 11.
    const std::array<double, 4> vec4 {-4.0, 4.0, 5.0, -8.0};
    const std::array<double, 4> c_vec4 {-4.0, 4.0, 5.0, -8.0};
    const std::array<double, 4> expected {
        -4.0 / 11.0, // -0.3636363636363636
        4.0 / 11.0,  //  0.3636363636363636
        5.0 / 11.0,  //  0.4545454545454545
        -8.0 / 11.0  // -0.7272727272727273
    };
    EXPECT_THAT(MathUtils::unit_vector(vec4), Pointwise(DoubleNear(tolerance), expected));
    EXPECT_THAT(MathUtils::unit_vector(c_vec4), Pointwise(DoubleNear(tolerance), expected));

    // Unit vector of a vector with magnitude zero.
    const std::array<double, 10> vec10 {};
    const double c_vec10[10] {};
    const std::array<double, 10> zero {};
    EXPECT_THAT(MathUtils::unit_vector(vec10), Pointwise(Eq(), zero));
    EXPECT_THAT(MathUtils::unit_vector(c_vec10), Pointwise(Eq(), zero));
}

// Test zeroing vectors.
TEST(MathUtils, ZeroVector) {
    using testing::Eq;
    using testing::Pointwise;

    std::array<double, 4> arr {1.0, 2.0, 3.33333, -4.4};
    double c_arr[4] {1.0, 2.0, 3.33333, -4.4};
    const std::array<double, 4> zero {};

    MathUtils::zero_vector(arr);
    MathUtils::zero_vector(c_arr);
    EXPECT_THAT(MathUtils::unit_vector(arr), Pointwise(Eq(), zero));
    EXPECT_THAT(MathUtils::unit_vector(c_arr), Pointwise(Eq(), zero));
}

// Test the element-wise square root functions.
TEST(MathUtils, VectorElementWiseSquareRoot) {
    using testing::DoubleNear;
    using testing::Pointwise;

    const std::array<double, 4> arr {4.0, 1.0, 16.0, 25.0};
    const double c_arr[4] {4.0, 1.0, 16.0, 25.0};
    const std::array<double, 4> expected {2.0, 1.0, 4.0, 5.0};
    EXPECT_THAT(MathUtils::vector_elementwise_sqrt(arr), Pointwise(DoubleNear(tolerance), expected));
    EXPECT_THAT(MathUtils::vector_elementwise_sqrt(c_arr), Pointwise(DoubleNear(tolerance), expected));

    double output[4] {};
    MathUtils::vector_elementwise_sqrt(c_arr, output);
    EXPECT_THAT(output, Pointwise(DoubleNear(tolerance), expected));
}

// Test the cross product functions.
TEST(MathUtils, VectorCrossProduct) {
    using testing::DoubleNear;
    using testing::Pointwise;

    const std::array<double, 3> arr1 {-3.0, 2.5, 1.0};
    const std::array<double, 3> arr2 {7.0, 0.0, -3.5};
    const double c_arr1[3] {arr1[0], arr1[1], arr1[2]};
    const double c_arr2[3] {arr2[0], arr2[1], arr2[2]};

    const std::array<double, 3> arr1_cross_arr2 {-8.75, -3.5, -17.5};
    EXPECT_THAT(MathUtils::vector_cross_product(arr1, arr2), Pointwise(DoubleNear(tolerance), arr1_cross_arr2));
    EXPECT_THAT(MathUtils::vector_cross_product(c_arr1, arr2), Pointwise(DoubleNear(tolerance), arr1_cross_arr2));
    EXPECT_THAT(MathUtils::vector_cross_product(arr1, c_arr2), Pointwise(DoubleNear(tolerance), arr1_cross_arr2));
    EXPECT_THAT(MathUtils::vector_cross_product(c_arr1, c_arr2), Pointwise(DoubleNear(tolerance), arr1_cross_arr2));
}

// Test the scalar product functions.
TEST(MathUtils, VectorScalarProduct) {

}

// Test the functions which swap a vector/matrix between a 1D vector and 2D matrix.
TEST(MathUtils, ConvertBetweenOneDimensionalVectorAndMatrix) {

}

// Test the various matrix copy functions.
TEST(MathUtils, MatrixCopyOperations) {

}

// Test the various matrix algebra functions.
TEST(MathUtils, MatrixAlgebra) {

}

// Test the various matrix multiplication functions.
TEST(MathUtils, MatrixMultiplication) {

}

// Test the function which generates a correlation matrix from a square covariance matrix.
TEST(MathUtils, CorrelationMatrix) {

}

}
