#include <gtest/gtest.h>
#include "structures/stack.h"

// Test fixture (optional, but useful if you want multiple tests)
class StackTest : public ::testing::Test {
protected:
    stack<int> s;
};

// Test that new stack is empty
TEST_F(StackTest, IsEmptyInitially) {
EXPECT_TRUE(s.empty());
EXPECT_EQ(s.size(), 0);
}

// Test push operation
TEST_F(StackTest, PushIncreasesSize) {
s.push(10);
EXPECT_FALSE(s.empty());
EXPECT_EQ(s.size(), 1);
EXPECT_EQ(s.top(), 10);

s.push(20);
EXPECT_EQ(s.size(), 2);
EXPECT_EQ(s.top(), 20);
}

// Test pop operation
TEST_F(StackTest, PopDecreasesSize) {
s.push(5);
s.push(15);

EXPECT_EQ(s.top(), 15);
s.pop();
EXPECT_EQ(s.top(), 5);
EXPECT_EQ(s.size(), 1);

s.pop();
EXPECT_TRUE(s.empty());
EXPECT_EQ(s.size(), 0);
}

// Test popping from empty stack throws
TEST_F(StackTest, PopOnEmptyThrows) {
EXPECT_THROW(s.pop(), std::runtime_error);
}

// Test top on empty stack throws
TEST_F(StackTest, TopOnEmptyThrows) {
EXPECT_THROW(s.top(), std::runtime_error);
}

// Test multiple push/pop
TEST_F(StackTest, PushPopSequence) {
for (int i = 0; i < 5; ++i) {
s.push(i * 10);
EXPECT_EQ(s.top(), i * 10);
}

for (int i = 4; i >= 0; --i) {
EXPECT_EQ(s.top(), i * 10);
s.pop();
}

EXPECT_TRUE(s.empty());
}
