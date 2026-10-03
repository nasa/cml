#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Floating point comparison tolerance.
constexpr double tolerance = 1e-12;

// Function to compare the values between two matrices.
template <std::size_t rows, std::size_t cols>
void test_matrices_equal(const double (&lhs)[rows][cols], const double (&rhs)[rows][cols]) {
    using testing::DoubleNear;
    using testing::Pointwise;
    for (unsigned int row = 0; row < rows; ++row) {
        EXPECT_THAT(lhs[row], Pointwise(DoubleNear(tolerance), rhs[row]));
    }
}

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
    const double c_vec4[4] {-4.0, 4.0, 5.0, -8.0};
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
    const std::array<double, 4> lhs {5.0, -3.0, 2.0, 6.0};
    const std::array<double, 4> rhs {-3.0, 1.0, 4.0, -2.0};
    const double c_lhs[4] {lhs[0], lhs[1], lhs[2], lhs[3]};
    const double c_rhs[4] {rhs[0], rhs[1], rhs[2], rhs[3]};

    ASSERT_NEAR(MathUtils::vector_scalar_product(lhs, rhs), -22.0, tolerance);
    ASSERT_NEAR(MathUtils::vector_scalar_product(c_lhs, rhs), -22.0, tolerance);
    ASSERT_NEAR(MathUtils::vector_scalar_product(lhs, c_rhs), -22.0, tolerance);
    ASSERT_NEAR(MathUtils::vector_scalar_product(c_lhs, c_rhs), -22.0, tolerance);

    double output = 0.0;
    MathUtils::vector_scalar_product(c_lhs, c_rhs, output);
    ASSERT_NEAR(output, -22.0, tolerance);
}

// Test the functions which swap a vector/matrix between a 1D vector and 2D matrix.
TEST(MathUtils, ConvertBetweenOneDimensionalVectorAndMatrix) {
    using testing::DoubleNear;
    using testing::Pointwise;

    // Set up the following matrix as a column-major and row-major vector:
    const double expected[4][3] {
       { 1.0, -2.0,  3.0},
       {-4.0,  5.5,  6.0},
       {-7.0, -8.0,  9.0},
       { 0.0, 10.0, -1.0}
    };
    const double col_major_vec[12] {1.0, -4.0, -7.0, 0.0, -2.0, 5.5, -8.0, 10.0, 3.0, 6.0, 9.0, -1.0};
    const double row_major_vec[12] {1.0, -2.0, 3.0, -4.0, 5.5, 6.0, -7.0, -8.0, 9.0, 0.0, 10.0, -1.0};

    double col_major_output[4][3] {};
    double row_major_output[4][3] {};
    MathUtils::col_maj_vec_to_matrix(col_major_vec, col_major_output);
    MathUtils::row_maj_vec_to_matrix(row_major_vec, row_major_output);

    test_matrices_equal(col_major_output, expected);
    test_matrices_equal(row_major_output, expected);
}

// Test the various matrix copy functions.
TEST(MathUtils, MatrixCopyOperations) {
    using testing::_;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    const double input[3][4] {
        {1.0, 2.0, 3.0, 4.0},
        {5.0, 6.0, 7.0, 8.0},
        {9.0, 10.0, 11.0, 12.0}
    };

    // Matrix copy
    {
        double output[3][4] {};
        MathUtils::matrix_copy(input, output);
        test_matrices_equal(input, output);
    }

    // Copy submatrix out
    {
        // Valid case.
        double output[2][2] {};
        const double expected1[2][2] {
            {6.0, 7.0},
            {10.0, 11.0}
        };
        MathUtils::matrix_copy_submatrix_out(input, output, 1, 1);
        test_matrices_equal(output, expected1);

        // Invalid case.
        const double expected2[2][2] {
            {8.0, 0.0},
            {12.0, 0.0}
        };
        EXPECT_CALL(cml_message_mock, publish(CMLMessage::Error, _, _, _));
        MathUtils::matrix_copy_submatrix_out(input, output, 1, 3);
        test_matrices_equal(output, expected2);
    }

    // Copy submatrix in
    {
        // Valid case.
        double output1[4][5] {
            {100.0, 200.0, 300.0, 400.0, 500.0},
            {600.0, 700.0, 800.0, 900.0, 1000.0},
            {1100.0, 1200.0, 1300.0, 1400.0, 1500.0},
            {1600.0, 1700.0, 1800.0, 1900.0, 2000.0}
        };
        const double expected1[4][5] {
            {100.0, 1.0, 2.0, 3.0, 4.0},
            {600.0, 5.0, 6.0, 7.0, 8.0},
            {1100.0, 9.0, 10.0, 11.0, 12.0},
            {1600.0, 1700.0, 1800.0, 1900.0, 2000.0}
        };
        MathUtils::matrix_copy_submatrix_in(input, output1, 0, 1);
        test_matrices_equal(output1, expected1);

        // Invalid case.
        double output2[4][5] {
            {100.0, 200.0, 300.0, 400.0, 500.0},
            {600.0, 700.0, 800.0, 900.0, 1000.0},
            {1100.0, 1200.0, 1300.0, 1400.0, 1500.0},
            {1600.0, 1700.0, 1800.0, 1900.0, 2000.0}
        };
        const double expected2[4][5] {
            {100.0, 200.0, 300.0, 400.0, 500.0},
            {600.0, 700.0, 800.0, 900.0, 1000.0},
            {1100.0, 1200.0, 1300.0, 1.0, 2.0},
            {1600.0, 1700.0, 1800.0, 5.0, 6.0}
        };
        EXPECT_CALL(cml_message_mock, publish(CMLMessage::Error, _, _, _));
        MathUtils::matrix_copy_submatrix_in(input, output2, 2, 3);
        test_matrices_equal(output2, expected2);
    }
}

// Test the various matrix algebra functions.
TEST(MathUtils, MatrixAlgebra) {
    // Zero matrix
    {
        double input[3][2] {
            {1.0, 2.0},
            {3.0, 4.0},
            {5.0, 6.0}
        };
        const double zeros[3][2] {};
        MathUtils::zero_matrix(input);
        test_matrices_equal(input, zeros);
    }

    // Increment
    {
        double input[3][2] {
            {1.0, 2.0},
            {3.0, 4.0},
            {5.0, 6.0}
        };
        const double increment[3][2] {
            {-1.0, 5.0},
            {2.0, -3.0},
            {0.0, 10.0}
        };
        const double expected[3][2] {
            {0.0, 7.0},
            {5.0, 1.0},
            {5.0, 16.0}
        };
        MathUtils::matrix_incr(increment, input);
        test_matrices_equal(input, expected);
    }

    // Decrement
    {
        double input[3][2] {
            {1.0, 2.0},
            {3.0, 4.0},
            {5.0, 6.0}
        };
        const double decrement[3][2] {
            {-1.0, 5.0},
            {2.0, -3.0},
            {0.0, 10.0}
        };
        const double expected[3][2] {
            {2.0, -3.0},
            {1.0, 7.0},
            {5.0, -4.0}
        };
        MathUtils::matrix_decr(decrement, input);
        test_matrices_equal(input, expected);
    }

    // Scale
    {
        double input[3][2] {
            {1.0, 2.0},
            {-0.5, 0.0},
            {-10.0, -11.0}
        };
        const double expected[3][2] {
            {2.0, 4.0},
            {-1.0, 0.0},
            {-20.0, -22.0}
        };
        MathUtils::matrix_scale(2.0, input);
        test_matrices_equal(input, expected);
    }
}

// Test the various matrix multiplication and miscellaneous functions.
TEST(MathUtils, MatrixOperations) {
    // Transpose
    {
        const double input[2][3] {
            {1.0, 2.0, 3.0},
            {4.0, 5.0, 6.0}
        };
        double output[3][2] {};
        const double expected[3][2] {
            {1.0, 4.0},
            {2.0, 5.0},
            {3.0, 6.0}
        };
        MathUtils::matrix_trans(input, output);
        test_matrices_equal(output, expected);
    }

    // Multiplication: L * R
    {
        const double lhs[3][2] {
            { 1.0, -2.0},
            {-3.0,  0.0},
            { 5.0,  3.0}
        };
        const double rhs[2][3] {
            {7.0, -3.0, 2.0},
            {-4.0, -10.0, 0.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {15.0, 17.0, 2.0},
            {-21.0, 9.0, -6.0},
            {23.0, -45.0, 10.0}
        };
        MathUtils::matrix_mult(lhs, rhs, output);
        test_matrices_equal(output, expected);
    }

    // Multiplication: L' * R
    {
        const double lhs[2][3] {
            {1.0, -3.0, 5.0},
            {-2.0, 0.0, 3.0}
        };
        const double rhs[2][3] {
            {7.0, -3.0, 2.0},
            {-4.0, -10.0, 0.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {15.0, 17.0, 2.0},
            {-21.0, 9.0, -6.0},
            {23.0, -45.0, 10.0}
        };
        MathUtils::matrix_mult_left_trans(lhs, rhs, output);
        test_matrices_equal(output, expected);
    }

    // Multiplication: L * R'
    {
        const double lhs[3][2] {
            { 1.0, -2.0},
            {-3.0,  0.0},
            { 5.0,  3.0}
        };
        const double rhs[3][2] {
            {7.0, -4.0},
            {-3.0, -10.0},
            {2.0, 0.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {15.0, 17.0, 2.0},
            {-21.0, 9.0, -6.0},
            {23.0, -45.0, 10.0}
        };
        MathUtils::matrix_mult_right_trans(lhs, rhs, output);
        test_matrices_equal(output, expected);
    }

    // Multiplication: L' * R'
    {
        const double lhs[3][2] {
            { 1.0, -2.0},
            {-3.0,  0.0},
            { 5.0,  3.0}
        };
        const double rhs[2][3] {
            {7.0, -3.0, 2.0},
            {-4.0, -10.0, 0.0}
        };
        double output[2][2] {};
        const double expected[2][2] {
            {26.0, 26.0},
            {-8.0, 8.0}
        };
        MathUtils::matrix_mult_trans_trans(lhs, rhs, output);
        test_matrices_equal(output, expected);
    }

    // Matrix transformations.
    {
        const double mat[3][3] {
            {1.0, -3.0, 2.0},
            {0.0, 4.0, 5.0},
            {-1.0, -2.0, 6.0}
        };
        const double trans[3][3] {
            {0.0, 1.0, 0.0},
            {-1.0, 0.0, 0.0},
            {0.0, 0.0, 1.0}
        };
        const double transformation_expected[3][3] {
            {4.0, 0.0, 5.0},
            {3.0, 1.0, -2.0},
            {-2.0, 1.0, 6.0}
        };
        const double inverse_transform_expected[3][3] {
            {4.0, 0.0, -5.0},
            {3.0, 1.0, 2.0},
            {2.0, -1.0, 6.0}
        };
        double output[3][3] {};

        MathUtils::matrix_transformation(trans, mat, output);
        test_matrices_equal(output, transformation_expected);

        MathUtils::matrix_inverse_transformation(trans, mat, output);
        test_matrices_equal(output, inverse_transform_expected);
    }

    // Matrix inverse transformation.
}

// Test the function which generates a correlation matrix from a square covariance matrix.
TEST(MathUtils, CorrelationMatrix) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Ordinary covariance matrix.
    {
        const double covariance[3][3] {
            {4.0, 100.0, 100.0},
            {2.0, 3.0, 100.0},
            {0.6, 0.9, 2.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {1.0, 0.5773502691896258, 0.2121320343559642},
            {0.5773502691896258, 1.0, 0.3674234614174767},
            {0.2121320343559642, 0.3674234614174767, 1.0}
        };

        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_TRUE(success);
        test_matrices_equal(output, expected);
    }

    // Positive variances and negative covariances.
    {
        const double covariance[3][3] {
            {4.0, 100.0, 100.0},
            {-1.2, 3.0, 100.0},
            {0.6, -0.9, 2.5}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {1.0, -0.3464101615137755, 0.1897366596101027},
            {-0.3464101615137755, 1.0, -0.3286335345030997},
            {0.1897366596101027, -0.3286335345030997, 1.0}
        };

        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_TRUE(success);
        test_matrices_equal(output, expected);
    }

    // Negative variances.
    {
        const double covariance[3][3] {
            {-1.0, 100.0, 100.0},
            {0.0, -2.0, 100.0},
            {0.0, 0.0, -3.0}
        };
        double output[3][3] {};
        const double expected[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("a diagonal element is negative")));
        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        ASSERT_FALSE(success);
        test_matrices_equal(output, expected);
    }

    // Only positive variances.
    {
        const double covariance[3][3] {
            {1.0, 100.0, 100.0},
            {0.0, 2.0, 100.0},
            {0.0, 0.0, 3.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {1.0, 0.0, 0.0},
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0}
        };

        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_TRUE(success);
        test_matrices_equal(output, expected);
    }

    // Zero variance and non-zero covariance in row.
    {
        const double covariance[3][3] {
            {1.0, 100.0, 100.0},
            {1.0, 0.0, 100.0},
            {0.0, 0.0, 1.0}
        };
        double output[3][3] {};
        const double expected[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("a diagonal element is zero\nwith non-zero off-diagonals")));
        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_FALSE(success);
        test_matrices_equal(output, expected);
    }

    // Zero variance and non-zero covariance in column.
    {
        const double covariance[3][3] {
            {1.0, 100.0, 100.0},
            {0.0, 0.0, 100.0},
            {0.0, 1.0, 1.0}
        };
        double output[3][3] {};
        const double expected[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("a diagonal element is zero\nwith non-zero off-diagonals")));
        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_FALSE(success);
        test_matrices_equal(output, expected);
    }

    // Zero variance and zero respective covariance.
    {
        const double covariance[3][3] {
            {0.0, 100.0, 100.0},
            {0.0, 1.0, 100.0},
            {0.0, 1.0, 1.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {1.0, 0.0, 0.0},
            {0.0, 1.0, 1.0},
            {0.0, 1.0, 1.0}
        };

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Inform, _, _, HasSubstr("diagonal element that is zero\nwith zero off-diagonals")));
        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_TRUE(success);
        test_matrices_equal(output, expected);
    }

    // All zero variance and covariances.
    {
        const double covariance[3][3] {
            {0.0, 100.0, 100.0},
            {0.0, 0.0, 100.0},
            {0.0, 0.0, 0.0}
        };
        double output[3][3] {};
        const double expected[3][3] {
            {1.0, 0.0, 0.0},
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0}
        };

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Inform, _, _, HasSubstr("diagonal element that is zero\nwith zero off-diagonals"))).Times(3);
        const bool success = MathUtils::extract_correlation_coefficients(covariance, output);
        EXPECT_TRUE(success);
        test_matrices_equal(output, expected);
    }

    // Transform PV matrix.
    {
        const double transform[3][3] {
            {0.0, 1.0, 0.0},
            {-1.0, 0.0, 0.0},
            {0.0, 0.0, 1.0}
        };
        const double pv_matrix[6][6] {
            {1.0, 2.0, 3.0, 4.0, 5.0, 6.0},
            {-2.0, 3.0, -4.0, 5.0, 6.0, 7.0},
            {0.5, 0.25, 0.1625, -1.0, -0.5, -0.6},
            {4.0, 2.0, 3.0, 1.0, 5.0, 9.0},
            {10.0, -11.0, 0.1, 0.4, 0.5, -0.6},
            {-5.0, -8.0, -20.0, 3.0, 5.0, 2.0}
        };
        const double expected[6][6] {
            { 3.0,   2.0,  -4.0,     6.0, -5.0,  7.0},
            {-2.0,   1.0,  -3.0,    -5.0,  4.0, -6.0},
            { 0.25, -0.5,   0.1625, -0.5,  1.0, -0.6},
            {-11.0, -10.0,  0.1,     0.5, -0.4, -0.6},
            {-2.0,   4.0,  -3.0,    -5.0,  1.0, -9.0},
            {-8.0,   5.0, -20.0,     5.0, -3.0,  2.0}
        };
        double output[6][6];
        MathUtils::transform_pv_matrix(transform, pv_matrix, output);
        test_matrices_equal(output, expected);
    }
}

}
