// Created by Omar on 11/24/2025.
//

#include <gtest/gtest.h>
#include "utils/helpers.h"

TEST(HelperTest, ToLowerCaseConvertsUpperCase) {
    EXPECT_EQ("hello", toLowerCase("HELLO"));
}

TEST(HelperTest, ToLowerCaseConvertsMixedCase) {
    EXPECT_EQ("hello world", toLowerCase("HeLLo WoRLd"));
}

TEST(HelperTest, ToLowerCaseHandlesAlreadyLowerCase) {
    EXPECT_EQ("hello", toLowerCase("hello"));
}

TEST(HelperTest, ToLowerCaseHandlesEmptyString) {
    EXPECT_EQ("", toLowerCase(""));
}

TEST(HelperTest, ToLowerCaseHandlesNumbers) {
    EXPECT_EQ("hello123", toLowerCase("HELLO123"));
}

TEST(HelperTest, ToLowerCaseHandlesSpecialCharacters) {
    EXPECT_EQ("hello-world!", toLowerCase("HELLO-WORLD!"));
}

TEST(HelperTest, ToLowerCaseHandlesSingleCharacter) {
    EXPECT_EQ("a", toLowerCase("A"));
}

TEST(HelperTest, ToLowerCaseHandlesSpaces) {
    EXPECT_EQ("hello world test", toLowerCase("HELLO WORLD TEST"));
}
