#include "../include/table_independent_variable.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {

// Double comparison tolerance for these tests.
inline constexpr double tolerance = 1e-8;

// Test the various constructors. Currently just for coverage.
TEST(TableIndependentVariable, Construction) {
    double variable {};
    const double eps = 1.0e-9;

    {
        TableIndependentVariable article(variable, eps);
        EXPECT_TRUE(article.get_name().empty());

        const std::string name = "VarName";
        article.set_name(name);
        EXPECT_EQ(name, article.get_name());
    }
    {
        const std::string name = "VarName";
        TableIndependentVariable article(name, variable, eps);
        EXPECT_EQ(name, article.get_name());

        // The model will complain, but will let us change the name.
        const std::string new_name = "DifferentName";
        article.set_name(new_name);
        EXPECT_EQ(new_name, article.get_name());
    }
    {
        TableIndependentVariable article(variable, TableIndependentVariable::Linear, eps);
        EXPECT_TRUE(article.get_name().empty());
    }
    {
        const std::string name = "VarName";
        TableIndependentVariable article(
            name,
            variable,
            TableIndependentVariable::WrapAround,
            eps);
        EXPECT_EQ(name, article.get_name());
    }
}

// Test loading invalid data.
TEST(TableIndependentVariable, LoadInvalidData) {
    double variable {};
    TableIndependentVariable article(variable);

    // Initializing before loading any data.
    EXPECT_FALSE(article.initialize());

    // Biasing and scaling before loading any data.
    article.bias_data(2.0, 0, 10);
    article.scale_data(2.0, 0, 10);

    // Updating before loading any data.
    EXPECT_FALSE(article.update());

    // Bad pointer.
    EXPECT_FALSE(article.load_data(nullptr, 100));
    EXPECT_FALSE(article.is_data_loaded());

    // Bad data size.
    const double some_data[] {0.0, 1.0, 2.0};
    EXPECT_FALSE(article.load_data(some_data, 0));
    EXPECT_FALSE(article.is_data_loaded());

    // Empty data.
    const std::vector<double> empty_data {};
    EXPECT_FALSE(article.load_data(empty_data));
    EXPECT_FALSE(article.is_data_loaded());

    // Non-monotonic data.
    const std::vector<double> non_monotonic_data {0.0, -1.0, 2.0, 0.0, 3.0};
    EXPECT_FALSE(article.load_data(non_monotonic_data));
    EXPECT_FALSE(article.is_data_loaded());

    // Duplicated values.
    const std::vector<double> duplicated_data {1.0, 1.0, 1.0};
    EXPECT_FALSE(article.load_data(duplicated_data));
    EXPECT_FALSE(article.is_data_loaded());
}

// Test reloading data after we've already loaded data.
TEST(TableIndependentVariable, ReloadData) {
    using ::testing::DoubleNear;
    using ::testing::Pointwise;

    double variable {};
    TableIndependentVariable article(variable);

    const std::vector<double> dataset1 {1.0, 2.0, 3.0};
    EXPECT_TRUE(article.load_data(dataset1));
    EXPECT_TRUE(article.is_data_loaded());
    EXPECT_TRUE(article.is_table_increasing());
    EXPECT_EQ(article.get_size(), dataset1.size());

    const std::vector<double> dataset2 {-1.0, -2.0, -3.0, -4.0};
    EXPECT_FALSE(article.load_data(dataset2));
    EXPECT_THAT(article.data, Pointwise(DoubleNear(1e-3), dataset1));

    article.clear_data();
    EXPECT_FALSE(article.is_data_loaded());

    EXPECT_TRUE(article.load_data(dataset2));
    EXPECT_TRUE(article.is_data_loaded());
    EXPECT_FALSE(article.is_table_increasing());
    EXPECT_EQ(article.get_size(), dataset2.size());
    EXPECT_THAT(article.data, Pointwise(DoubleNear(1e-3), dataset2));
}

// Test biasing and scaling the data.
TEST(TableIndependentVariable, BiasAndScale) {
    using ::testing::DoubleNear;
    using ::testing::Pointwise;

    double variable {};
    const std::vector<double> breakpoints {1.0, 2.0, 3.0, 4.0};

    // In these tests we'll attempt to multiply the data by 2 and add 0.1.
    const std::vector<double> expected_data {2.1, 4.1, 6.1, 8.1};

    {
        TableIndependentVariable article(variable);
        article.load_data(breakpoints);

        article.scale_data(2.0);
        article.bias_data(0.1);
        EXPECT_THAT(article.data, Pointwise(DoubleNear(1e-3), expected_data));
    }
    {
        // Same as the previous case except we mix up the upper and lower indices.
        // The model will graciously swap these values and still apply our bias
        // and scale factor.
        TableIndependentVariable article(variable);
        article.load_data(breakpoints);

        article.scale_data(2.0, breakpoints.size() - 1, 0);
        article.bias_data(0.1, breakpoints.size() - 1, 0);
        EXPECT_THAT(article.data, Pointwise(DoubleNear(1e-3), expected_data));
    }
    {
        // Try to modify the data in a way that would make the breakpoints non-
        // monotonic.
        //
        // Scaling the last element by 0.1 would result in breakpoints of:
        //     [1, 2, 3, 0.4]
        // Biasing the last element by -10 would result in breakpoints of:
        //     [1, 2, 3, -6]
        //
        // The model will reject these attempts and leave the original breakpoints
        // unchanged.
        TableIndependentVariable article(variable);
        article.load_data(breakpoints);

        article.scale_data(0.1, breakpoints.size() - 1);
        EXPECT_THAT(article.data, Pointwise(DoubleNear(1e-3), breakpoints));

        article.bias_data(-10.0, breakpoints.size() - 1);
        EXPECT_THAT(article.data, Pointwise(DoubleNear(1e-3), breakpoints));
    }
}

// Test data that's off the table's front and back.
TEST(TableIndependentVariable, OffTable) {
    double variable {};
    TableIndependentVariable article(variable);

    std::vector<double> breakpoints {0.0, 1.0, 2.0};
    article.load_data(breakpoints);
    article.initialize();

    variable = 0.5;
    article.update();
    EXPECT_FALSE(article.is_off_table());
    EXPECT_FALSE(article.is_off_table_front());
    EXPECT_FALSE(article.is_off_table_back());

    variable = -1.0;
    article.update();
    EXPECT_TRUE(article.is_off_table());
    EXPECT_TRUE(article.is_off_table_front());
    EXPECT_FALSE(article.is_off_table_back());

    // Note that if the independent variable is equal to the final breakpoint,
    // it is considered to be off the table back.
    variable = 2.0;
    article.update();
    EXPECT_TRUE(article.is_off_table());
    EXPECT_FALSE(article.is_off_table_front());
    EXPECT_TRUE(article.is_off_table_back());
}

// Test various lookups with a data size of 1.
TEST(TableIndependentVariable, OneBreakpoint) {
    double variable {};
    const std::vector<double> breakpoints {1.0};
    TableIndependentVariable article(variable);
    article.load_data(breakpoints);
    article.initialize();

    // No matter what value the independent variable holds, the index of the
    // calibrated data will always be zero.
    variable = 0.0;
    article.update();
    EXPECT_EQ(article.get_index(), 0);
    EXPECT_NEAR(article.fraction, 0.0, tolerance);

    variable = 1.0;
    article.update();
    EXPECT_EQ(article.get_index(), 0);
    EXPECT_NEAR(article.fraction, 0.0, tolerance);

    variable = 2.0;
    article.update();
    EXPECT_EQ(article.get_index(), 0);
    EXPECT_NEAR(article.fraction, 0.0, tolerance);
}

// Test various lookups with a data size of 2.
TEST(TableIndependentVariable, TwoBreakpoints) {
    double variable {};
    TableIndependentVariable article(variable);

    const std::vector<double> breakpoints {0.0, 1.0};
    article.load_data(breakpoints);
    article.initialize();

    variable = -1.0;
    article.update();
    ASSERT_EQ(article.get_index(), 0);
    ASSERT_NEAR(article.fraction, 0.0, tolerance);
    ASSERT_TRUE(article.is_off_table_front());

    variable = 0.5;
    article.update();
    ASSERT_EQ(article.get_index(), 0);
    ASSERT_NEAR(article.fraction, 0.5, tolerance);

    variable = 1.0;
    article.update();
    ASSERT_EQ(article.get_index(), 1);
    ASSERT_EQ(article.fraction, 0.0);
    ASSERT_TRUE(article.is_off_table_back());

    variable = 1.2;
    article.update();
    ASSERT_EQ(article.get_index(), 1);
    ASSERT_EQ(article.fraction, 0.0);
    ASSERT_TRUE(article.is_off_table_back());

    variable = 0.9;
    article.update();
    ASSERT_EQ(article.get_index(), 0);
    ASSERT_NEAR(article.fraction, 0.9, tolerance);
}

// Test the WrapAround continuity option
TEST(TableIndependentVariable, WrapAroundContinuity) {
    double variable {};
    const double eps = 1e-3;
    TableIndependentVariable article(variable, TableIndependentVariable::WrapAround, eps);

    // Set breakpoints starting at 0 and ending at 2 with an interval of 0.1.
    // The large number of breakpoints exercises the binary search functionality.
    std::vector<double> breakpoints(21);
    for (unsigned index = 0; index < breakpoints.size(); ++index) {
        breakpoints[index] = 0.1 * static_cast<double>(index);
    }
    article.load_data(breakpoints);
    article.initialize();

    // Data is in-bounds.
    variable = 1.1;
    article.update();
    ASSERT_NEAR(article.fraction, 0.0, tolerance);
    ASSERT_EQ(article.get_index(), 11);

    // Data wraps around the front of the table, resolving to 1.45.
    variable = -0.55;
    article.update();
    ASSERT_NEAR(article.fraction, 0.5, tolerance);
    ASSERT_EQ(article.get_index(), 14);

    // Data wraps around the back of the table, resolving to 1.0.
    variable = 3.0;
    article.update();
    ASSERT_NEAR(article.fraction, 0.0, tolerance);
    ASSERT_EQ(article.get_index(), 10);

    // Test a huge wrap-around. Also switch to performing a full search.
    article.perform_full_search = true;
    variable = 101.0;
    article.update();
    ASSERT_NEAR(article.fraction, 0.0, tolerance);
    ASSERT_EQ(article.get_index(), 10);
    variable = -101.0;
    article.update();
    ASSERT_NEAR(article.fraction, 0.0, tolerance);
    ASSERT_EQ(article.get_index(), 10);

    // Test a value very close to the upper bin.
    variable = 1.0 - 0.01 * eps;
    article.update();
    ASSERT_TRUE(article.prox_override);
    ASSERT_EQ(article.index_prox, 10);
}

}
