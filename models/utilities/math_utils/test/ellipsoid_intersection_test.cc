#include "../include/ellipsoid_intersection.hh"

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

// Test several updates of the ellipsoid intersection model.
TEST(EllipsoidIntersection, Update) {
    using testing::DoubleNear;
    using testing::Pointwise;

    // Floating point comparison tolerance.
    constexpr double tolerance = 1e-12;

    // Case 1
    {
        const double source_point[3] {5.0, 0.0, 0.0};
        const double target_point[3] {-5.0, 0.0, 0.0};

        EllipsoidIntersection article(source_point, target_point, 2.0, 3.0, 4.0);
        const bool intersection = article.update();
        EXPECT_TRUE(intersection);
        EXPECT_THAT(article.root1, Pointwise(DoubleNear(tolerance), {2.0, 0.0, 0.0}));
        EXPECT_THAT(article.root2, Pointwise(DoubleNear(tolerance), {-2.0, 0.0, 0.0}));
        EXPECT_NEAR(article.get_scaled_root1(), 0.3, tolerance);
        EXPECT_NEAR(article.get_scaled_root2(), 0.7, tolerance);
    }

    // Case 2
    {
        const double source_point[3] {0.0, 5.0, 0.0};
        const double target_point[3] {0.0, -5.0, 0.0};

        EllipsoidIntersection article(source_point, target_point, 2.0, 3.0, 4.0);
        const bool intersection = article.update();
        EXPECT_TRUE(intersection);
        EXPECT_THAT(article.root1, Pointwise(DoubleNear(tolerance), {0.0, 3.0, 0.0}));
        EXPECT_THAT(article.root2, Pointwise(DoubleNear(tolerance), {0.0, -3.0, 0.0}));
        EXPECT_NEAR(article.get_scaled_root1(), 0.2, tolerance);
        EXPECT_NEAR(article.get_scaled_root2(), 0.8, tolerance);
    }

    // Case 3
    {
        const double source_point[3] {0.0, 0.0, 5.0};
        const double target_point[3] {0.0, 0.0, -5.0};

        EllipsoidIntersection article(source_point, target_point, 2.0, 3.0, 4.0);
        const bool intersection = article.update();
        EXPECT_TRUE(intersection);
        EXPECT_THAT(article.root1, Pointwise(DoubleNear(tolerance), {0.0, 0.0, 4.0}));
        EXPECT_THAT(article.root2, Pointwise(DoubleNear(tolerance), {0.0, 0.0, -4.0}));
        EXPECT_NEAR(article.get_scaled_root1(), 0.1, tolerance);
        EXPECT_NEAR(article.get_scaled_root2(), 0.9, tolerance);
    }

    // Case 4
    {
        const double source_point[3] {-1.0, 0.0, 4.0};
        const double target_point[3] {1.0, 0.0, 4.0};

        EllipsoidIntersection article(source_point, target_point, 2.0, 3.0, 4.0);
        const bool intersection = article.update();
        EXPECT_TRUE(intersection);
        EXPECT_THAT(article.root1, Pointwise(DoubleNear(tolerance), {0.0, 0.0, 4.0}));
        EXPECT_THAT(article.root2, Pointwise(DoubleNear(tolerance), {0.0, 0.0, 4.0}));
        EXPECT_NEAR(article.get_scaled_root1(), 0.5, tolerance);
        EXPECT_NEAR(article.get_scaled_root2(), 0.5, tolerance);
    }

    // Case 5
    {
        const double source_point[3] {1.0, 1.0, 1.0};
        const double target_point[3] {0.0, 0.0, 1.0};

        EllipsoidIntersection article(source_point, target_point, 2.0, 3.0, 4.0);
        const bool intersection = article.update();
        EXPECT_TRUE(intersection);
        EXPECT_THAT(article.root1, Pointwise(DoubleNear(tolerance), {1.611258466588724, 1.611258466588724, 1.0}));
        EXPECT_THAT(article.root2, Pointwise(DoubleNear(tolerance), {-1.611258466588724, -1.611258466588724, 1.0}));
        EXPECT_NEAR(article.get_scaled_root1(), -0.61125846658872407, tolerance);
        EXPECT_NEAR(article.get_scaled_root2(), 2.611258466588724, tolerance);
    }

    // Case 6
    {
        const double source_point[3] {1.5, 2.5, 0.0};
        const double target_point[3] {1.5, 2.5, 1.0};

        EllipsoidIntersection article(source_point, target_point, 2.0, 3.0, 4.0);
        const bool intersection = article.update(false);
        EXPECT_FALSE(intersection);
    }
}

}
