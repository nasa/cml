#include <cstdlib>
#include <exception>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../include/unit_test.hh" 
#include "mocks/cml/cml_message_mock.hh"

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
    setenv("MY_TEST_PATH", "/tmp/testdir", 1);
    UnitTestFrameworkTest utf;

    std::string input = "${MY_TEST_PATH}/file.txt";
    std::string expected = "/tmp/testdir/file.txt";

    EXPECT_EQ(utf.expand_env_variables(input), expected);
}

TEST(UnitTestFrameworkTest, ExpandEnvVariableUnknownThrows) {
    using testing::_;
    using testing::HasSubstr;

    CMLMessage::Mock cml_message_mock;

    unsetenv("NON_EXISTENT_VAR");
    UnitTestFrameworkTest utf;

    EXPECT_CALL(
        cml_message_mock,
        publish(CMLMessage::Error, _, _, HasSubstr("'NON_EXISTENT_VAR' is not set")));
    std::string input = "${NON_EXISTENT_VAR}/file.txt";
    EXPECT_THROW(utf.expand_env_variables(input), std::runtime_error);
}
