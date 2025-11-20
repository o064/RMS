//
// Created by Omar on 11/20/2025.
//

#include <gtest/gtest.h>
#include "models/Train.h"
#include "models/SeatAllocator.h"

std::string captureCoutOutput(std::function<void()> func) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    func();
    std::cout.rdbuf(old);
    return buffer.str();
}
class TrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        train1 = new Train(1, "Express", 5);
        train2 = new Train(2, "Local", 0); // should default to 10 seats
    }

    void TearDown() override {
        delete train1;
        delete train2;
    }

    Train* train1;
    Train* train2;
};

// Test Train constructor and seat allocator
TEST_F(TrainTest, ConstructorAndSeatAllocator) {
    EXPECT_EQ(train1->getTrainId(), 1);
    EXPECT_EQ(train1->getTrainName(), "Express");
    EXPECT_EQ(train1->getSeatAllocator()->getAvailableSeatCount(), 5);

    // Train with 0 seats defaults to 10
    EXPECT_EQ(train2->getSeatAllocator()->getAvailableSeatCount(), 10);
}

// Test hasAvailableSeats
TEST_F(TrainTest, HasAvailableSeats) {
    EXPECT_TRUE(train1->hasAvailbeSeats());

    // Allocate all seats
    for (int i = 0; i < 5; ++i) {
        train1->getSeatAllocator()->allocateSeat(100 + i);
    }
    EXPECT_FALSE(train1->hasAvailbeSeats());
}

// Test get and set train name
TEST_F(TrainTest, GetSetTrainName) {
    EXPECT_EQ(train1->getTrainName(), "Express");
    train1->setTrainName("Super Express");
    EXPECT_EQ(train1->getTrainName(), "Super Express");
}

// Test seat allocation through Train
TEST_F(TrainTest, AllocateSeatThroughTrain) {
    auto allocator = train1->getSeatAllocator();
    int seat = allocator->allocateSeat(101);
    EXPECT_EQ(seat, 1);
    EXPECT_EQ(allocator->getAvailableSeatCount(), 4);
}

// Test freeing seat through Train
TEST_F(TrainTest, FreeSeatThroughTrain) {
    auto allocator = train1->getSeatAllocator();
    int seat = allocator->allocateSeat(101);
    allocator->freeSeat(seat);
    EXPECT_EQ(allocator->getAvailableSeatCount(), 5);
}

// Test duplicate passenger allocation through Train
TEST_F(TrainTest, DuplicatePassengerID) {
    auto allocator = train1->getSeatAllocator();
    int seat = allocator->allocateSeat(101);
    EXPECT_EQ(seat, 1);

    std::string output = captureCoutOutput([&]() {
        int duplicateSeat = allocator->allocateSeat(101);
        EXPECT_EQ(duplicateSeat, -1);
    });
    EXPECT_TRUE(output.find("Passenger ID 101 is already allocated a seat.") != std::string::npos);
}


