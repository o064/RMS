#include <gtest/gtest.h>
#include "utils/helpers.h"

// ---------- toLowerCase ----------
TEST(HelpersTest, ToLowerCaseBasic) {
    EXPECT_EQ(toLowerCase("HELLO"), "hello");
    EXPECT_EQ(toLowerCase("HeLLo"), "hello");
}

TEST(HelpersTest, ToLowerCaseEmpty) {
    EXPECT_EQ(toLowerCase(""), "");
}

// ---------- trim ----------
TEST(HelpersTest, TrimBasic) {
    EXPECT_EQ(trim("  hello  "), "hello");
    EXPECT_EQ(trim("world"), "world");
}

TEST(HelpersTest, TrimOnlySpaces) {
    EXPECT_EQ(trim("     "), "");
}

TEST(HelpersTest, TrimEdgeCases) {
    EXPECT_EQ(trim(" a "), "a");
    EXPECT_EQ(trim("   abc"), "abc");
    EXPECT_EQ(trim("xyz   "), "xyz");
}

// ---------- parseInt ----------
TEST(HelpersTest, ParseIntValid) {
    EXPECT_EQ(parseInt("123", "age"), 123);
    EXPECT_EQ(parseInt("-45", "value"), -45);
}

TEST(HelpersTest, ParseIntInvalidArgument) {
    EXPECT_THROW(parseInt("abc", "age"), std::invalid_argument);
    EXPECT_THROW(parseInt("12a", "num"), std::invalid_argument);
}

TEST(HelpersTest, ParseIntOutOfRange) {
    // Very large number
    EXPECT_THROW(parseInt("999999999999999999999999", "big"), std::out_of_range);
}

// ---------- isValidName ----------
TEST(HelpersTest, ValidNames) {
    EXPECT_TRUE(isValidName("Omar"));
    EXPECT_TRUE(isValidName("Jean-Luc"));
    EXPECT_TRUE(isValidName("D'Angelo"));
    EXPECT_TRUE(isValidName("John Doe"));
}

TEST(HelpersTest, InvalidNames) {
    EXPECT_FALSE(isValidName(" Omar"));     // leading space
    EXPECT_FALSE(isValidName("Omar "));     // trailing space
    EXPECT_FALSE(isValidName("Om@r"));      // symbol
    EXPECT_FALSE(isValidName("123"));       // digits
    EXPECT_FALSE(isValidName(""));          // empty
    EXPECT_FALSE(isValidName("A--B"));      // weird pattern
}

// ---------- combineString ----------
TEST(HelpersTest, CombineStringValid) {
    std::vector<std::string> args = {"add", "user", "Omar", "Mohamed"};
    EXPECT_EQ(combineString(args, 2), "Omar Mohamed");
}

TEST(HelpersTest, CombineStringInvalidNameThrows) {
    std::vector<std::string> args = {"add", "user", "Omar", "M@hamed"};
    EXPECT_THROW(combineString(args, 2), std::runtime_error);
}

TEST(HelpersTest, CombineStringEdgeCases) {
    std::vector<std::string> args = {"cmd", "name", "Jean-Luc", "Pierre"};
    EXPECT_EQ(combineString(args, 2), "Jean-Luc Pierre");
}
