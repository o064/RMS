#include <gtest/gtest.h>
#include "models/Passenger.h"

class PassengerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ==================== VALID CONSTRUCTORS ====================
TEST_F(PassengerTest, ConstructorSetsIdAndName) {
    Passenger p(1, "John Doe");
    EXPECT_EQ(1, p.getId());
    EXPECT_EQ("John Doe", p.getName());
}

TEST_F(PassengerTest, SetIdUpdatesId) {
    Passenger p(1, "Jane Smith");
    p.setId(42);
    EXPECT_EQ(42, p.getId());
}

TEST_F(PassengerTest, NameWithSpecialCharacters) {
    Passenger p(1, "O'Brien-Smith");
    EXPECT_EQ("O'Brien-Smith", p.getName());
}

TEST_F(PassengerTest, NameWithSpaces) {
    Passenger p(1, "John Middle Doe");
    EXPECT_EQ("John Middle Doe", p.getName());
}

TEST_F(PassengerTest, NameWithNumbers) {
    Passenger p(1, "Agent007");
    EXPECT_EQ("Agent007", p.getName());
}

TEST_F(PassengerTest, SetIdMultipleTimes) {
    Passenger p(1, "Test");
    p.setId(10);
    EXPECT_EQ(10, p.getId());
    p.setId(20);
    EXPECT_EQ(20, p.getId());
}

TEST_F(PassengerTest, LongName) {
    std::string longName = "VeryLongPassengerNameThatExceedsNormalLength";
    Passenger p(1, longName);
    EXPECT_EQ(longName, p.getName());
}

// ==================== INVALID CONSTRUCTORS ====================

// ID < 0 should throw

TEST_F(PassengerTest, ConstructorWithNegativeIdThrows) {
    EXPECT_THROW(Passenger p(-1, "Invalid Passenger"), std::runtime_error);
}

// Empty name should throw
TEST_F(PassengerTest, EmptyNameThrows) {
    EXPECT_THROW(Passenger p(1, ""), std::runtime_error);
}
