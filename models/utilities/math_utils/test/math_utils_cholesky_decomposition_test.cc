#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <cmath>
#include <cstddef>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Floating point comparison tolerance.
inline constexpr double tolerance = 1e-12;

// Function to compare the values between two matrices.
template <std::size_t dimension>
void test_matrices_equal(const double (&lhs)[dimension][dimension], const double (&rhs)[dimension][dimension]) {
    using testing::DoubleNear;
    using testing::Pointwise;
    for (unsigned int row = 0; row < dimension; ++row) {
        EXPECT_THAT(lhs[row], Pointwise(DoubleNear(tolerance), rhs[row]));
    }
}

// Test various null pointer failures.
TEST(MathUtils, CholeskyDecompositionNullPointers) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    constexpr unsigned int dimension = 10;
    const double in_array[dimension][dimension] {};
    double out_array[dimension][dimension] {};

    // Null input array.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("the matrix to be decomposed is addressed with a NULL pointer")));
    EXPECT_FALSE(MathUtils::cholesky_decomposition("Test", nullptr, &out_array[0][0], dimension));

    // Null output array.
    EXPECT_CALL(cml_message_mock,
        publish(CMLMessage::Error, _, _,
            HasSubstr("array to which the computed decomposition will be sent is addressed with a NULL pointer")));
    EXPECT_FALSE(MathUtils::cholesky_decomposition("Test", &in_array[0][0], nullptr, dimension));
}

// Test calculation of Cholesky decomposition of a 3x3 matrix.
TEST(MathUtils, CholeskyDecomposition3x3) {
    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    constexpr unsigned int dimension = 3;
    const double in_array[dimension][dimension] {
        {1.0, 0.0, 1.0},
        {0.0, 2.0, 0.0},
        {1.0, 0.0, 3.0}
    };
    double out_array[dimension][dimension] {};
    const double expected[dimension][dimension] {
      {1.0, 0.0, 0.0},
      {0.0, std::sqrt(2.0), 0.0},
      {1.0, 0.0, std::sqrt(2.0)}
    };

    EXPECT_CALL(cml_message_mock, publish).Times(0);
    const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension);
    EXPECT_TRUE(success);
    test_matrices_equal(out_array, expected);
}

// Test calculation of Cholesky decomposition of a submatrix within a larger matrix.
TEST(MathUtils, CholeskyDecompositionSubmatrix) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Decompose a 3x3 submatrix within a 10x10 matrix.
    {
        // Values outside the upper 3x3 region shouldn't have any effect on the output.
        constexpr unsigned int dimension = 10;
        const double in_array[dimension][dimension] {
            {1.0, 0.0, 1.0, 100.0},
            {0.0, 2.0, 0.0, 200.0},
            {1.0, 0.0, 3.0, 300.0},
            {-50.0, -60.0, 0.0, -1.0}
        };
        double out_array[dimension][dimension] {};
        const double expected[dimension][dimension] {
            {1.0, 0.0, 0.0},
            {0.0, std::sqrt(2.0), 0.0},
            {1.0, 0.0, std::sqrt(2.0)}
        };

        EXPECT_CALL(cml_message_mock, publish).Times(0);
        const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension, 3);
        EXPECT_TRUE(success);
        test_matrices_equal(out_array, expected);
    }

    // Attempt to decompose a submatrix that's larger than the input matrix.
    {
        constexpr unsigned int dimension = 5;
        const double in_array[dimension][dimension] {};
        double out_array[dimension][dimension] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("A sub-matrix must be smaller than the original matrix")));
        const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension, dimension + 1);
        EXPECT_FALSE(success);
    }
}

// Test various Cholesky decomposition error conditions
TEST(MathUtils, CholeskyDecompositionErrorConditions) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Asymmetric matrix.
    {
        constexpr unsigned int dimension = 3;
        const double in_array[dimension][dimension] {
            {1.0, -5000.0, 200.0},
            {0.0, 2.0, 4444.0},
            {1.0, 0.0, 3.0}
        };
        double out_array[dimension][dimension] {};

        // The decomposition will proceed with values below the diagonal, treating the
        // matrix as though it was symmetric.
        const double expected[dimension][dimension] {
            {1.0, 0.0, 0.0},
            {0.0, std::sqrt(2.0), 0.0},
            {1.0, 0.0, std::sqrt(2.0)}
        };

        EXPECT_CALL(cml_message_mock,
                publish(CMLMessage::Error, _, _, HasSubstr("not a symmetric matrix")));
        const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension);
        EXPECT_TRUE(success);
        test_matrices_equal(out_array, expected);
    }

    // Negative determinant.
    {
        constexpr unsigned int dimension = 3;
        const double in_array[dimension][dimension] {
            {1.0, 0.0, 1.0},
            {0.0, 2.0, 0.0},
            {1.0, 0.0, 0.0}
        };
        double out_array[dimension][dimension] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _,
                HasSubstr("A negative eigen-value exists, detected in processing col 2")));
        const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension);
        EXPECT_FALSE(success);
    }

    // Zero determinant.
    {
        constexpr unsigned int dimension = 3;
        const double in_array[dimension][dimension] {
            {0.0, 0.0, 1.0},
            {0.0, 2.0, 0.0},
            {1.0, 0.0, 3.0}
        };
        double out_array[dimension][dimension] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _,
                HasSubstr("A negative eigen-value exists\n")));
        const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension);
        EXPECT_FALSE(success);
    }

    // Ambiguous solution due to determinant of zero.
    {
        constexpr unsigned int dimension = 10;
        const double in_array[dimension][dimension] {
            { 1.6641,   3.50751,  0.74175,  0,  1.73118,   2.64321,   0.73788,   1.67184,   0.84108,   1.24485  },
            { 3.50751,  8.013905, 3.187493, 0,  4.57795,   7.662583,  2.00364,   3.573468,  2.857864,  4.637963 },
            { 0.74175,  3.187493, 4.82635,  0,  3.773771,  7.531023,  2.654479,  1.877517,  3.946451,  6.775963 },
            { 0,        0,        0,        0,  0,         0,         0,         0,         0,         0        },
            { 1.73118,  4.57795,  3.773771, 0,  9.967224,  14.664142, 10.295208, 12.423878, 9.559074,  9.055285 },
            { 2.64321,  7.662583, 7.531023, 0,  14.664142, 22.981239, 14.016988, 17.331281, 14.986778, 16.304499},
            { 0.73788,  2.00364,  2.654479, 0,  10.295208, 14.016988, 25.62034,  20.296328, 17.423248, 10.169399},
            { 1.67184,  3.573468, 1.877517, 0,  12.423878, 17.331281, 20.296328, 32.862215, 16.117226, 14.139477},
            { 0.84108,  2.857864, 3.946451, 0,  9.559074,  14.986778, 17.423248, 16.117226, 25.962729, 17.506381},
            { 1.24485,  4.637963, 6.775963, 0,  9.055285,  16.304499, 10.169399, 14.139477, 17.506381, 27.0338  }
        };
        double out_array[dimension][dimension] {};
        const double expected[dimension][dimension] {
            { 1.29,  0,     0,     0, 0,                 0,                   0,                  0,                  0,                 0                 },
            { 2.719, 0.788, 0,     0, 0,                 0,                   0,                  0,                  0,                 0                 },
            { 0.575, 2.061, 0.498, 0, 0,                 0,                   0,                  0,                  0,                 0                 },
            { 0,     0,     0,     0, 0,                 0,                   0,                  0,                  0,                 0                 },
            { 1.342, 1.179, 1.149, 0, 2.33581206435792,  0,                   0,                  0,                  0,                 0                 },
            { 2.049, 2.654, 1.773, 0, 2.888991414578969, 0.4993211456457813,  0,                  0,                  0,                 0                 },
            { 0.572, 0.569, 2.315, 0, 2.652952304920735, -0.8692150051890255, 3.437510631630103,  0,                  0,                 0                 },
            { 1.296, 0.063, 2.013, 0, 3.552268663481212, 1.355945637049325,   1.923974545593591,  2.994592079019146,  0,                 0                 },
            { 0.652, 1.377, 1.473, 0, 2.298185749578112, 1.492434492028816,   2.343866284839132, -0.827036959989062,  2.790170111847167, 0                 },
            { 0.965, 2.556, 1.914, 0, 1.090646391836446, 2.001116809004143,   0.7499941599377179, 0.2819358021403084, 1.261754093264061, 2.911711272835517 }
        };

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _,
                HasSubstr("There are multiple solutions for the decomposition of this matrix")));
        const bool success = MathUtils::cholesky_decomposition("Test", &in_array[0][0], &out_array[0][0], dimension);
        EXPECT_TRUE(success);
        test_matrices_equal(out_array, expected);
    }
}

}
