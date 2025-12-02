#include <gtest/gtest.h>
#include "models/Passenger.h"

class PassengerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PassengerTest, ValidConstruction) {
    Passenger p(1, "John Doe");
    EXPECT_EQ(p.getId(), 1);
    EXPECT_EQ(p.getName(), "John Doe");
}

TEST_F(PassengerTest, InvalidIdThrowsException) {
    EXPECT_THROW(Passenger(-1, "John Doe"), std::invalid_argument);
}

TEST_F(PassengerTest, EmptyNameThrowsException) {
    EXPECT_THROW(Passenger(1, ""), std::invalid_argument);
}

TEST_F(PassengerTest, SetValidId) {
    Passenger p(1, "John Doe");
    p.setId(2);
    EXPECT_EQ(p.getId(), 2);
}

TEST_F(PassengerTest, SetInvalidIdThrowsException) {
    Passenger p(1, "John Doe");
    EXPECT_THROW(p.setId(-1), std::invalid_argument);
}

TEST_F(PassengerTest, SetValidName) {
    Passenger p(1, "John Doe");
    p.setName("Jane Smith");
    EXPECT_EQ(p.getName(), "Jane Smith");
}

TEST_F(PassengerTest, SetEmptyNameThrowsException) {
    Passenger p(1, "John Doe");
    EXPECT_THROW(p.setName(""), std::invalid_argument);
}