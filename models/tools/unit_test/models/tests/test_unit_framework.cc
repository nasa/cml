#include <gtest/gtest.h>
#include "../include/unit_test.hh"

TEST(UnitTestFramework, DefaultConstructorInitializesState) {
    UnitTestFramework utf;

    EXPECT_TRUE(utf.enabled);
    EXPECT_FALSE(utf.cycle_data);
    EXPECT_EQ(utf.vars_file_name, "");
    EXPECT_EQ(utf.data_file_name, "");
    EXPECT_EQ(utf.linked_vars_file_name, "");
    EXPECT_EQ(utf.cycle_overruns_limit, 2u);
}
