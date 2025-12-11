#include <gtest/gtest.h>
#include "utils/helpers.h"
#include <regex>

TEST(HelpersTest, TrimFunction) {
    EXPECT_EQ("hello", trim("hello"));
    EXPECT_EQ("hello", trim("  hello"));
    EXPECT_EQ("hello", trim("hello  "));
    EXPECT_EQ("hello", trim("  hello  "));
    EXPECT_EQ("", trim(""));
    EXPECT_EQ("", trim("   "));
    EXPECT_EQ("hello world", trim("  hello world  "));
}

TEST(HelpersTest, ToLowerCaseFunction) {
    EXPECT_EQ("hello", toLowerCase("HELLO"));
    EXPECT_EQ("hello", toLowerCase("Hello"));
    EXPECT_EQ("123", toLowerCase("123"));
    EXPECT_EQ("", toLowerCase(""));
    EXPECT_EQ("hello world", toLowerCase("HELLO WORLD"));
}

TEST(HelpersTest, ParseIntFunction) {
    EXPECT_EQ(123, parseInt("123", "ID"));
    EXPECT_EQ(-50, parseInt("-50", "Balance"));
    EXPECT_THROW(parseInt("12a", "ID"), std::invalid_argument);
    EXPECT_THROW(parseInt("abc", "ID"), std::invalid_argument);
    EXPECT_THROW(parseInt("", "ID"), std::invalid_argument);
    EXPECT_THROW(parseInt("   ", "ID"), std::invalid_argument);
    EXPECT_THROW(parseInt("12.5", "ID"), std::invalid_argument);
    EXPECT_THROW(parseInt("9999999999999999999", "ID"), std::out_of_range);
}

TEST(HelpersTest, IsValidNameFunction) {
    EXPECT_TRUE(isValidName("John"));
    EXPECT_TRUE(isValidName("Sarah Connor"));
    EXPECT_TRUE(isValidName("Jean-Luc"));
    EXPECT_TRUE(isValidName("O'Neil"));
//    EXPECT_FALSE(isValidName("John123"));
    EXPECT_FALSE(isValidName(" John"));
    EXPECT_FALSE(isValidName("John "));
    EXPECT_FALSE(isValidName(""));
    EXPECT_FALSE(isValidName("-John"));
    EXPECT_FALSE(isValidName("John--Doe"));
}

TEST(HelpersTest, CombineStringFunction) {
    const vector<std::string> args = {"cmd", "John", "Doe", "Smith"};
    const vector<std::string> invalidArgs = {"cmd", "John", "123"};

    EXPECT_EQ("John Doe Smith", combineString(args, 1));
    EXPECT_EQ("Doe Smith", combineString(args, 2));
    EXPECT_THROW(combineString(invalidArgs, 1), std::runtime_error);
    EXPECT_EQ("John Doe", combineString(args, 1, 3));
    EXPECT_THROW(combineString(args, 10, 12), std::runtime_error);
    EXPECT_THROW(combineString(args, 2, 1), std::runtime_error);
}

TEST(HelpersTest, CompareStringFunction) {
    EXPECT_TRUE(compareString("Hello", "hello"));
    EXPECT_TRUE(compareString("  JoHn  ", "john"));
    EXPECT_FALSE(compareString("Apple", "Banana"));
}

TEST(HelpersTest, IsIntegerFunction) {
    EXPECT_TRUE(isInteger("123"));
    EXPECT_TRUE(isInteger("-123"));
    EXPECT_FALSE(isInteger("12a"));
    EXPECT_FALSE(isInteger("abc"));
    EXPECT_TRUE(isInteger(" 123 "));
}