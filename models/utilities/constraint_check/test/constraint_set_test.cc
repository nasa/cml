#include "../include/constraint.hh"
#include "../include/constraint_set.hh"
#include <gtest/gtest.h>

namespace {

// Test stub which does nothing.
class ConstraintUnitTest : public Constraint {
public:
    void update() override {}
};

// Test a regular update.
TEST(ConstraintSet, Update) {
    ConstraintSet article;
    ConstraintUnitTest constraint1;
    ConstraintUnitTest constraint2;
    constraint1.name = "Constraint 1";
    constraint2.name = "Constraint 2";
    article.constraints.push_back(&constraint1);
    article.constraints.push_back(&constraint2);

    article.initialize();
    article.subscribe();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_TRUE(article.is_active());
    EXPECT_FALSE(article.constraint_disabled(&constraint1));
    EXPECT_FALSE(article.constraint_disabled(&constraint2));

    // The dummy constraints have no logic to determine whether they're violated or not.
    // Inject conditions for 0, 1, and both constraints being violated.
    article.update();
    EXPECT_EQ(article.get_violated_constraints(), "");

    constraint1.violated = true;
    article.update();
    EXPECT_EQ(article.get_violated_constraints(), "Constraint 1");

    constraint1.violated = true;
    constraint2.violated = true;
    article.update();
    EXPECT_EQ(article.get_violated_constraints(), "Constraint 1, Constraint 2");
}

// Test an update where the constraint set is not active.
TEST(ConstraintSet, Inactive) {
    ConstraintSet article;
    ConstraintUnitTest constraint1;
    ConstraintUnitTest constraint2;
    constraint1.name = "Constraint 1";
    constraint2.name = "Constraint 2";
    article.constraints.push_back(&constraint1);
    article.constraints.push_back(&constraint2);

    article.initialize();
    EXPECT_TRUE(article.is_initialized());
    EXPECT_FALSE(article.is_active());
    EXPECT_FALSE(article.constraint_disabled(&constraint1));
    EXPECT_FALSE(article.constraint_disabled(&constraint2));

    // Even though Constraint 1 is violated, it will not be reported since the set
    // is not active.
    constraint1.violated = true;
    article.update();
    EXPECT_EQ(article.get_violated_constraints(), "");
}

}
