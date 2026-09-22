#include "../include/quadratic_solver.hh"

#include <gtest/gtest.h>

namespace {

// Floating point comparison tolerance
inline constexpr double tolerance = 1e-12;

// Test the quadratic solver's root finding math.
TEST(QuadraticSolver, ComputeRoots) {
    // x^2-3x+2=0
    {
        QuadraticSolver article(1.0, -3.0, 2.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, 1.0, tolerance);
        EXPECT_NEAR(article.root2, 2.0, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // x^2+2x+1=0
    {
        QuadraticSolver article(1.0, 2.0, 1.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, -1.0, tolerance);
        EXPECT_NEAR(article.root2, -1.0, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // 2x+1=0
    {
        QuadraticSolver article(0.0, 2.0, 1.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, -0.5, tolerance);
        EXPECT_NEAR(article.root2, -0.5, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // x^2-1=0
    {
        QuadraticSolver article(1.0, 0.0, -1.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, -1.0, tolerance);
        EXPECT_NEAR(article.root2, 1.0, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // x^2+3x=0
    {
        QuadraticSolver article(1.0, 3.0, 0.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, -3.0, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // 1=0
    {
        QuadraticSolver article(0.0, 0.0, 1.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, 0.0, tolerance);
        EXPECT_FALSE(article.roots_exist);
        EXPECT_FALSE(success);
    }

    // 3x=0
    {
        QuadraticSolver article(0.0, 3.0, 0.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, 0.0, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // 3x^2=0
    {
        QuadraticSolver article(3.0, 0.0, 0.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, 0.0, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }

    // 0=0
    {
        QuadraticSolver article(0.0, 0.0, 0.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, 0.0, tolerance);
        EXPECT_FALSE(article.roots_exist);
        EXPECT_FALSE(success);
    }

    // x^2+(1e308)x+1=0
    {
        QuadraticSolver article(1.0, 1e308, 1.0);
        const bool success = article.solve();
        EXPECT_NEAR(article.root1, -1e-308, tolerance);
        EXPECT_NEAR(article.root2, -1e308, tolerance);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);
    }
}

// Test the quadratic solver's logic for finding whether roots exist, but not
// actually solving for them.
TEST(QuadraticSolver, DontComputeRoots) {
    // x^2-3x+2=0
    {
        QuadraticSolver article(1.0, 3.0, 2.0, false);
        const bool success = article.solve(false);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);

        // Roots aren't calculated. They'd be -2 and -1 if we solved for them.
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, 0.0, tolerance);
    }

    // 2x+1=0
    {
        QuadraticSolver article(0.0, 2.0, 1.0, false);
        const bool success = article.solve(false);
        EXPECT_TRUE(article.roots_exist);
        EXPECT_TRUE(success);

        // Roots aren't calculated. They'd be -0.5 and -0.5 if we solved for them.
        EXPECT_NEAR(article.root1, 0.0, tolerance);
        EXPECT_NEAR(article.root2, 0.0, tolerance);
    }
}

}
