#include "../include/math_utils.hh"
#include "mocks/cml/cml_message_mock.hh"

#include <cmath>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Common input values for each transformation matrix generation test case. For most
// tests, vector1 is position and vector2 is velocity.
inline constexpr double vector1_test_cases[][3] {
    {0.0,   0.0,   0.0},
    {200.0, 100.0, 0.0},
    {100.0, 0.0,   0.0},
    {0.0,   0.0,   200.0},
    {200.0, 100.0, 300.0},
    {200.0, 100.0, 300.0},
    {0.0,   0.0,   10.0},
};
inline constexpr double vector2_test_cases[][3] {
    {10.0,  10.0,  10.0},
    {0.0,   0.0,   0.0},
    {0.0,   0.0,   0.0},
    {0.0,   0.0,   0.0},
    {10.0,  30.0,  20.0},
    {200.0, 100.0, 300.0},
    {0.0,   0.0,   10.0},
};

// Target to use for identity matrix comparisons. Just to avoid re-creating this
// a bunch of times.
inline constexpr double identity_matrix[3][3] {
    {1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0}
};

// Floating point comparison tolerance.
inline constexpr double tolerance = 1e-12;

// Function to compare the values between two matrices.
void test_matrices_equal(const double (&lhs)[3][3], const double (&rhs)[3][3]) {
    using testing::DoubleNear;
    using testing::Pointwise;
    for (unsigned int row = 0; row < 3; ++row) {
        EXPECT_THAT(lhs[row], Pointwise(DoubleNear(tolerance), rhs[row]));
    }
}

// Test the method which generates an inertial to LVLH frame transformation matrix.
TEST(MathUtils, GenerateInertialToLvlh) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Null pointer cases.
    {
        const double position[3] {};
        const double velocity[3] {};
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("output matrix is NULL")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, nullptr);

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("input arguments are NULL"))).Times(3);
        MathUtils::generate_inertial_to_lvlh(nullptr, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, identity_matrix);

        MathUtils::generate_inertial_to_lvlh(position, nullptr, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, identity_matrix);

        MathUtils::generate_inertial_to_lvlh(nullptr, nullptr, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, identity_matrix);
    }

    // Case 1
    {
        const double (&position)[3] = vector1_test_cases[0];
        const double (&velocity)[3] = vector2_test_cases[0];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning LVLH with inertial instead")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }

    // Case 2
    {
        const double (&position)[3] = vector1_test_cases[1];
        const double (&velocity)[3] = vector2_test_cases[1];
        constexpr double expected[3][3] {
            {0.4472135954999579, -0.8944271909999157, 0.0},
            {0.0,                0.0,                 1.0},
            {-0.894427190999916, -0.447213595499958,  0.0}
        };
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning LVLH-x close to inertial-x")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }

    // Case 3
    {
        const double (&position)[3] = vector1_test_cases[2];
        const double (&velocity)[3] = vector2_test_cases[2];
        constexpr double expected[3][3] {
            {0.0,  0.0, 1.0},
            {0.0,  1.0, 0.0},
            {-1.0, 0.0, 0.0}
        };
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning LVLH-y close to inertial-y")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }

    // Case 4
    {
        const double (&position)[3] = vector1_test_cases[3];
        const double (&velocity)[3] = vector2_test_cases[3];
        constexpr double expected[3][3] {
            {1.0,  0.0,  0.0},
            {0.0, -1.0,  0.0},
            {0.0,  0.0, -1.0}
        };
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning LVLH-x close to inertial-x")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }

    // Case 5
    {
        const double (&position)[3] = vector1_test_cases[4];
        const double (&velocity)[3] = vector2_test_cases[4];
        constexpr double expected[3][3] {
            {-0.2468853599393471,  0.95668076976497,   -0.1543033499620919},
            {0.8082903768654761,   0.1154700538379252, -0.5773502691896257},
            {-0.5345224838248488, -0.2672612419124244, -0.8017837257372732}
        };
        double T_inrtl_lvlh[3][3] {};

        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }

    // Case 6
    {
        const double (&position)[3] = vector1_test_cases[5];
        const double (&velocity)[3] = vector2_test_cases[5];
        constexpr double expected[3][3] {
            {0.8451542547285165,  -0.1690308509457033, -0.5070925528371099},
            {0.0,                 -0.9486832980505137, 0.3162277660168379},
            {-0.5345224838248488, -0.2672612419124244, -0.8017837257372732}
        };
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning LVLH-x close to inertial-x")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }

    // Case 7
    {
        const double (&position)[3] = vector1_test_cases[6];
        const double (&velocity)[3] = vector2_test_cases[6];
        constexpr double expected[3][3] {
            {1.0,  0.0,  0.0},
            {0.0, -1.0,  0.0},
            {0.0,  0.0, -1.0}
        };
        double T_inrtl_lvlh[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning LVLH-x close to inertial-x")));
        MathUtils::generate_inertial_to_lvlh(position, velocity, T_inrtl_lvlh);
        test_matrices_equal(T_inrtl_lvlh, expected);
    }
}

// Test the method which generates an inertial to UVW frame transformation matrix.
TEST(MathUtils, GenerateInertialToUvw) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Null pointer cases.
    {
        const double position[3] {};
        const double velocity[3] {};
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("output matrix is NULL")));
        MathUtils::generate_inertial_to_uvw(position, velocity, nullptr);

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("input arguments are NULL"))).Times(3);
        MathUtils::generate_inertial_to_uvw(nullptr, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, identity_matrix);

        MathUtils::generate_inertial_to_uvw(position, nullptr, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, identity_matrix);

        MathUtils::generate_inertial_to_uvw(nullptr, nullptr, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, identity_matrix);
    }

    // Case 1
    {
        const double (&position)[3] = vector1_test_cases[0];
        const double (&velocity)[3] = vector2_test_cases[0];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning UVW with inertial instead")));
        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }

    // Case 2
    {
        const double (&position)[3] = vector1_test_cases[1];
        const double (&velocity)[3] = vector2_test_cases[1];
        const double expected[3][3] {
            {0.894427190999916,  0.447213595499958, 0.0},
            {-0.447213595499958, 0.894427190999916, 0.0},
            {0.0,                0.0,               1.0}
        };
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning UVW-w close to inertial-z")));
        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }

    // Case 3
    {
        const double (&position)[3] = vector1_test_cases[2];
        const double (&velocity)[3] = vector2_test_cases[2];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning UVW-w close to inertial-z")));
        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }

    // Case 4
    {
        const double (&position)[3] = vector1_test_cases[3];
        const double (&velocity)[3] = vector2_test_cases[3];
        const double expected[3][3] {
            {0.0, 0.0,  1.0},
            {0.0, -1.0, 0.0},
            {1.0, 0.0,  0.0}
        };
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning UVW-w close to inertial-x")));
        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }

    // Case 5
    {
        const double (&position)[3] = vector1_test_cases[4];
        const double (&velocity)[3] = vector2_test_cases[4];
        const double expected[3][3] {
            {0.5345224838248488,  0.2672612419124244,  0.8017837257372732},
            {-0.2468853599393471, 0.95668076976497,    -0.1543033499620919},
            {-0.8082903768654761, -0.1154700538379252, 0.5773502691896257}
        };
        double T_inrtl_uvw[3][3] {};

        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }

    // Case 6
    {
        const double (&position)[3] = vector1_test_cases[5];
        const double (&velocity)[3] = vector2_test_cases[5];
        const double expected[3][3] {
            {0.5345224838248488,   0.2672612419124244, 0.8017837257372732},
            {-0.447213595499958,   0.894427190999916,  0.0},
            {-0.7171371656006362, -0.3585685828003181, 0.5976143046671969}
        };
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning UVW-w close to inertial-z")));
        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }

    // Case 7
    {
        const double (&position)[3] = vector1_test_cases[6];
        const double (&velocity)[3] = vector2_test_cases[6];
        const double expected[3][3] {
            {0.0, 0.0,  1.0},
            {0.0, -1.0, 0.0},
            {1.0, 0.0,  0.0}
        };
        double T_inrtl_uvw[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Aligning UVW-w close to inertial-x")));
        MathUtils::generate_inertial_to_uvw(position, velocity, T_inrtl_uvw);
        test_matrices_equal(T_inrtl_uvw, expected);
    }
}

// Test the method which generates an inertial to arbitrary reference frame transformation matrix.
TEST(MathUtils, GenerateInertialToReference) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Null pointer cases.
    {
        const double x_axis_inertial[3] {};
        const double position[3] {};
        double T_inrtl_reference[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("output matrix is NULL")));
        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, nullptr);

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("input arguments are NULL"))).Times(3);
        MathUtils::generate_inrtl_to_reference(nullptr, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, identity_matrix);

        MathUtils::generate_inrtl_to_reference(x_axis_inertial, nullptr, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, identity_matrix);

        MathUtils::generate_inrtl_to_reference(nullptr, nullptr, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, identity_matrix);
    }

    // Case 1
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[0];
        const double (&position)[3] = vector2_test_cases[0];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_reference[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning reference ref-frame with inertial frame instead")));
        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }

    // Case 2
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[1];
        const double (&position)[3] = vector2_test_cases[1];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_reference[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning reference ref-frame with inertial frame instead")));
        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }

    // Case 3
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[2];
        const double (&position)[3] = vector2_test_cases[2];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_reference[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning reference ref-frame with inertial frame instead")));
        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }

    // Case 4
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[3];
        const double (&position)[3] = vector2_test_cases[3];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_reference[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning reference ref-frame with inertial frame instead")));
        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }

    // Case 5
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[4];
        const double (&position)[3] = vector2_test_cases[4];
        const double expected[3][3] {
            {0.5345224838248488,   0.2672612419124244, 0.8017837257372732},
            {-0.8082903768654761, -0.1154700538379251, 0.5773502691896257},
            {0.246885359939347,   -0.95668076976497,   0.1543033499620919}
        };
        double T_inrtl_reference[3][3] {};

        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }

    // Case 6
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[5];
        const double (&position)[3] = vector2_test_cases[5];
        const double expected[3][3] {
            {0.5345224838248488,  0.2672612419124244, 0.8017837257372732},
            {0.4472135954999579, -0.8944271909999159, 0.0},
            {0.7171371656006362,  0.3585685828003181, -0.5976143046671968}
        };
        double T_inrtl_reference[3][3] {};

        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }

    // Case 7
    {
        const double (&x_axis_inertial)[3] = vector1_test_cases[6];
        const double (&position)[3] = vector2_test_cases[6];
        const double expected[3][3] {
            {0.0,  0.0, 1.0},
            {0.0,  1.0, 0.0},
            {-1.0, 0.0, 0.0}
        };
        double T_inrtl_reference[3][3] {};

        MathUtils::generate_inrtl_to_reference(x_axis_inertial, position, T_inrtl_reference);
        test_matrices_equal(T_inrtl_reference, expected);
    }
}

// Test the method which generates an inertial to VNC frame transformation matrix.
TEST(MathUtils, GenerateInertialToVnc) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Case 1
    {
        const double (&position)[3] = vector1_test_cases[0];
        const double (&velocity)[3] = vector2_test_cases[0];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_vnc[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("either the position or velocity is a zero vector")));
        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }

    // Case 2
    {
        const double (&position)[3] = vector1_test_cases[1];
        const double (&velocity)[3] = vector2_test_cases[1];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_vnc[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("either the position or velocity is a zero vector")));
        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }

    // Case 3
    {
        const double (&position)[3] = vector1_test_cases[2];
        const double (&velocity)[3] = vector2_test_cases[2];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_vnc[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("either the position or velocity is a zero vector")));
        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }

    // Case 4
    {
        const double (&position)[3] = vector1_test_cases[3];
        const double (&velocity)[3] = vector2_test_cases[3];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_vnc[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("either the position or velocity is a zero vector")));
        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }

    // Case 5
    {
        const double (&position)[3] = vector1_test_cases[4];
        const double (&velocity)[3] = vector2_test_cases[4];
        const double expected[3][3] {
            {0.2672612419124243,  0.8017837257372731,  0.5345224838248487},
            {-0.8082903768654761, -0.1154700538379252, 0.5773502691896257},
            {0.5246313898711126,  -0.5863527298559493, 0.6172133998483676}
        };
        double T_inrtl_vnc[3][3] {};

        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }

    // Case 6
    {
        const double (&position)[3] = vector1_test_cases[5];
        const double (&velocity)[3] = vector2_test_cases[5];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_vnc[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("position and velocity are aligned")));
        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }

    // Case 7
    {
        const double (&position)[3] = vector1_test_cases[6];
        const double (&velocity)[3] = vector2_test_cases[6];
        const double (&expected)[3][3] = identity_matrix;
        double T_inrtl_vnc[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("position and velocity are aligned")));
        MathUtils::generate_inrtl_to_vnc(position, velocity, T_inrtl_vnc);
        test_matrices_equal(T_inrtl_vnc, expected);
    }
}

// Test the method which generates a planet-fixed to topocentric East-North-Up frame transformation matrix.
TEST(MathUtils, GenerateTransPfixToEnu) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    // Null pointer cases.
    {
        const double position_pfix[3] {};
        double T_pfix_enu[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("output matrix is NULL")));
        MathUtils::generate_T_pfix_to_enu(position_pfix, nullptr);

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Position vector is NULL")));
        MathUtils::generate_T_pfix_to_enu(nullptr, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, identity_matrix);
    }

    // Case 1
    {
        const double (&position_pfix)[3] = vector1_test_cases[0];
        const double expected[3][3] = {};
        double T_pfix_enu[3][3] {};

        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }

    // Case 2
    {
        const double (&position_pfix)[3] = vector1_test_cases[1];
        const double expected[3][3] = {
            {-0.447213595499958, 0.894427190999916, 0.0},
            {0.0,                0.0,               1.0},
            {0.894427190999916,  0.447213595499958, 0.0}
        };
        double T_pfix_enu[3][3] {};

        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }

    // Case 3
    {
        const double (&position_pfix)[3] = vector1_test_cases[2];
        const double expected[3][3] = {
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0},
            {1.0, 0.0, 0.0}
        };
        double T_pfix_enu[3][3] {};

        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }

    // Case 4
    {
        const double (&position_pfix)[3] = vector1_test_cases[3];
        const double (&expected)[3][3] = identity_matrix;
        double T_pfix_enu[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning east with pfix +x")));
        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }

    // Case 5
    {
        const double (&position_pfix)[3] = vector1_test_cases[4];
        const double expected[3][3] = {
            {-0.4472135954999579, 0.8944271909999159,  0.0},
            {-0.7171371656006362, -0.3585685828003181, 0.5976143046671968},
            {0.5345224838248488,  0.2672612419124244,  0.8017837257372732}
        };
        double T_pfix_enu[3][3] {};

        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }

    // Case 6
    {
        const double (&position_pfix)[3] = vector1_test_cases[5];
        const double expected[3][3] = {
            {-0.4472135954999579, 0.8944271909999159,  0.0},
            {-0.7171371656006362, -0.3585685828003181, 0.5976143046671968},
            {0.5345224838248488,  0.2672612419124244,  0.8017837257372732}
        };
        double T_pfix_enu[3][3] {};

        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }

    // Case 7
    {
        const double (&position_pfix)[3] = vector1_test_cases[6];
        const double (&expected)[3][3] = identity_matrix;
        double T_pfix_enu[3][3] {};

        EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Error, _, _, HasSubstr("Aligning east with pfix +x")));
        MathUtils::generate_T_pfix_to_enu(position_pfix, T_pfix_enu);
        test_matrices_equal(T_pfix_enu, expected);
    }
}

// Test the method which generates a generic East-North-Up to planet-fixed frame transformation quaternion.
TEST(MathUtils, GenerateQuatEnuToPfix) {
    using testing::_;
    using testing::HasSubstr;

    testing::StrictMock<CMLMessage::Mock> cml_message_mock;

    jeod::Quaternion q_enu_to_pfix;

    // Case 1
    MathUtils::generate_Q_enu_to_pfix(0.0, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.5, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.5, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.5, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.5, tolerance);

    // Case 2
    MathUtils::generate_Q_enu_to_pfix(1.570796, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 1.15539443734658e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 1.15539443734658e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.7071067811865381, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.7071067811865381, tolerance);

    // Case 3
    MathUtils::generate_Q_enu_to_pfix(0.0, 1.570796, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.7071067811865381, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 1.15539443734658e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 1.15539443734658e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.7071067811865381, tolerance);

    // Case 4
    MathUtils::generate_Q_enu_to_pfix(1.570796, 1.570796, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 1.633974483494782e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 2.670086374223501e-14, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 1.633974483494782e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.9999999999999732, tolerance);

    // Case 5
    EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Latitude too large")));
    MathUtils::generate_Q_enu_to_pfix(1.570796, 1.570797, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, -0.9999999999999302, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], -3.366025517481663e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 5.498379529456088e-14, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 1.633974485437673e-07, tolerance);

    // Case 6
    EXPECT_CALL(cml_message_mock,
            publish(CMLMessage::Warning, _, _, HasSubstr("Latitude too small")));
    MathUtils::generate_Q_enu_to_pfix(1.570796, -1.570797, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, -3.366025517204108e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], -0.9999999999999302, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 1.633974485715228e-07, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 5.498379529456088e-14, tolerance);

    // Case 7
    MathUtils::generate_Q_enu_to_pfix(0.523599, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.3535533218845634, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.3535533218845634, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.6123724753647819, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.6123724753647819, tolerance);

    // Case 8
    MathUtils::generate_Q_enu_to_pfix(0.0, 0.523599, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.6123724753647819, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.3535533218845634, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.3535533218845634, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.6123724753647819, tolerance);

    // Case 9
    MathUtils::generate_Q_enu_to_pfix(0.523599, 0.523599, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.4330126457917832, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.2499999028312194, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.4330126457917831, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.7500000971687807, tolerance);

    // Case 10
    MathUtils::generate_Q_enu_to_pfix(0.785398, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.2705981034453612, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.2705981034453612, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.6532814603306707, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.6532814603306707, tolerance);

    // Case 11
    MathUtils::generate_Q_enu_to_pfix(0.0, 0.785398, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.6532814603306707, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.2705981034453612, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.2705981034453612, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.6532814603306707, tolerance);

    // Case 12
    MathUtils::generate_Q_enu_to_pfix(1.0472, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.1830118656101681, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.1830118656101681, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.6830129259727709, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.6830129259727709, tolerance);

    // Case 13
    MathUtils::generate_Q_enu_to_pfix(0.0, 1.0472, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.6830129259727709, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.1830118656101681, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.1830118656101681, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.6830129259727709, tolerance);

    // Case 14
    MathUtils::generate_Q_enu_to_pfix(0.0, -1.0472, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.1830118656101681, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.6830129259727709, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.6830129259727709, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.1830118656101681, tolerance);

    // Case 15
    MathUtils::generate_Q_enu_to_pfix(2.0944, -0.523599, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, -0.1294106800827339, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], -0.2241459310253017, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.836515809039842, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.4829625023868248, tolerance);

    // Case 16
    MathUtils::generate_Q_enu_to_pfix(4.1888, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, -0.6830135982082818, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], -0.6830135982082818, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], 0.1830093567623682, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], 0.1830093567623682, tolerance);

    // Case 17
    MathUtils::generate_Q_enu_to_pfix(-2.0944, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.6830122537280444, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.6830122537280444, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], -0.1830143744554987, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], -0.1830143744554987, tolerance);

    // Case 18
    MathUtils::generate_Q_enu_to_pfix(10.472, 0.0, q_enu_to_pfix);
    EXPECT_NEAR(q_enu_to_pfix.scalar, 0.6830149426516572, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[0], 0.6830149426516572, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[1], -0.1830043390593605, tolerance);
    EXPECT_NEAR(q_enu_to_pfix.vector[2], -0.1830043390593605, tolerance);
}

}
