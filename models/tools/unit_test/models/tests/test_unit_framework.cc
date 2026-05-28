#include <gtest/gtest.h>
#include "../include/unit_test.hh" 

// Create a test subclass to access protected methods (if needed)
class UnitTestFrameworkTest : public UnitTestFramework {
public:
    using UnitTestFramework::expand_env_variables;
};

TEST(UnitTestFrameworkTest, DefaultConstructorInitializesState) {
    UnitTestFramework utf;

    EXPECT_TRUE(utf.enabled);
    EXPECT_FALSE(utf.cycle_data);
    EXPECT_EQ(utf.vars_file_name, "");
    EXPECT_EQ(utf.data_file_name, "");
    EXPECT_EQ(utf.linked_vars_file_name, "");
    EXPECT_EQ(utf.cycle_overruns_limit, 2u);
}

TEST(UnitTestFrameworkTest, ExpandEnvVariableKnown) {
    setenv("MY_TEST_PATH", "/tmp/testdir", 1);  // POSIX-compatible
    UnitTestFrameworkTest utf;

    std::string input = "${MY_TEST_PATH}/file.txt";
    std::string expected = "/tmp/testdir/file.txt";

    EXPECT_EQ(utf.expand_env_variables(input), expected);
}

TEST(UnitTestFrameworkTest, ExpandEnvVariableUnknownThrows) {
    unsetenv("NON_EXISTENT_VAR");
    UnitTestFrameworkTest utf;

    std::string input = "${NON_EXISTENT_VAR}/file.txt";

    try {
        utf.expand_env_variables(input);
        FAIL() << "Expected std::runtime_error due to missing environment variable";
    } catch (const std::runtime_error& e) {
        std::string msg = e.what();
        EXPECT_NE(msg.find("NON_EXISTENT_VAR"), std::string::npos)
            << "Error message should mention the missing variable name";
    } catch (...) {
        FAIL() << "Expected std::runtime_error but caught a different exception type";
    }
}