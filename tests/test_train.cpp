#include <gtest/gtest.h>
#include "models/Train.h"
#include "models/SeatAllocator.h"
#include <sstream>
#include <iostream>
#include <functional>

// Helper function to capture cout output
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
        train2 = new Train(2, "Local", 0);
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
    EXPECT_EQ(train1->getTotalSeats(), 5);
    EXPECT_EQ(train1->getSeatAllocator()->getAvailableSeatCount(), 5);

    // Train with 0 seats defaults to 10
    EXPECT_EQ(train2->getTotalSeats(), 0);  // totalSeats stores the value passed
    EXPECT_EQ(train2->getSeatAllocator()->getAvailableSeatCount(), 10);  // but allocator defaults to 10
}

// Test hasAvailableSeats
TEST_F(TrainTest, HasAvailableSeats) {
    EXPECT_TRUE(train1->hasAvailableSeats());

    // Allocate all seats
    auto allocator = train1->getSeatAllocator();
    for (int i = 0; i < 5; ++i) {
        allocator->allocateSeat(100 + i);
    }
    EXPECT_FALSE(train1->hasAvailableSeats());
}

// Test get and set train name
TEST_F(TrainTest, GetSetTrainName) {
    EXPECT_EQ(train1->getTrainName(), "Express");
    train1->setTrainName("Super Express");
    EXPECT_EQ(train1->getTrainName(), "Super Express");
}

// Test get and set train ID
TEST_F(TrainTest, GetSetTrainId) {
    EXPECT_EQ(train1->getTrainId(), 1);
    train1->setTrainId(100);
    EXPECT_EQ(train1->getTrainId(), 100);
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
    EXPECT_EQ(allocator->getAvailableSeatCount(), 4);

    captureCoutOutput([&]() {
        allocator->freeSeat(seat);
    });
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
    EXPECT_TRUE(output.find("Passenger ID 101 is already allocated a seat") != std::string::npos);
}

// Test copy constructor
TEST_F(TrainTest, CopyConstructor) {
    // Allocate some seats first
    train1->getSeatAllocator()->allocateSeat(101);
    train1->getSeatAllocator()->allocateSeat(102);

    Train copiedTrain(*train1);

    EXPECT_EQ(copiedTrain.getTrainId(), train1->getTrainId());
    EXPECT_EQ(copiedTrain.getTrainName(), train1->getTrainName());
    EXPECT_EQ(copiedTrain.getTotalSeats(), train1->getTotalSeats());
    // Seat allocator state should be copied too
    EXPECT_EQ(copiedTrain.getSeatAllocator()->getAvailableSeatCount(),
              train1->getSeatAllocator()->getAvailableSeatCount());
}

// Test copy assignment operator
TEST_F(TrainTest, CopyAssignment) {
    // Allocate some seats first
    train1->getSeatAllocator()->allocateSeat(101);

    Train assignedTrain(3, "Another", 10);
    assignedTrain = *train1;

    EXPECT_EQ(assignedTrain.getTrainId(), train1->getTrainId());
    EXPECT_EQ(assignedTrain.getTrainName(), train1->getTrainName());
    EXPECT_EQ(assignedTrain.getTotalSeats(), train1->getTotalSeats());
    EXPECT_EQ(assignedTrain.getSeatAllocator()->getAvailableSeatCount(),
              train1->getSeatAllocator()->getAvailableSeatCount());
}

// Test clone method
TEST_F(TrainTest, CloneMethod) {
    train1->getSeatAllocator()->allocateSeat(101);

    auto cloned = train1->clone();

    EXPECT_EQ(cloned->getTrainId(), train1->getTrainId());
    EXPECT_EQ(cloned->getTrainName(), train1->getTrainName());
    EXPECT_EQ(cloned->getTotalSeats(), train1->getTotalSeats());
    EXPECT_EQ(cloned->getSeatAllocator()->getAvailableSeatCount(),
              train1->getSeatAllocator()->getAvailableSeatCount());

    // Verify it's a deep copy - changes to original shouldn't affect clone
    train1->getSeatAllocator()->allocateSeat(102);
    EXPECT_NE(cloned->getSeatAllocator()->getAvailableSeatCount(),
              train1->getSeatAllocator()->getAvailableSeatCount());
}

// Test move constructor
TEST_F(TrainTest, MoveConstructor) {
    Train* tempTrain = new Train(10, "Temp", 8);
    tempTrain->getSeatAllocator()->allocateSeat(101);

    Train movedTrain(std::move(*tempTrain));

    EXPECT_EQ(movedTrain.getTrainId(), 10);
    EXPECT_EQ(movedTrain.getTrainName(), "Temp");
    EXPECT_EQ(movedTrain.getTotalSeats(), 8);
    EXPECT_EQ(movedTrain.getSeatAllocator()->getAvailableSeatCount(), 7);

    delete tempTrain;
}

// Test move assignment
TEST_F(TrainTest, MoveAssignment) {
    Train movedTrain(20, "Initial", 3);

    Train tempTrain(10, "Temp", 8);
    tempTrain.getSeatAllocator()->allocateSeat(101);

    movedTrain = std::move(tempTrain);

    EXPECT_EQ(movedTrain.getTrainId(), 10);
    EXPECT_EQ(movedTrain.getTrainName(), "Temp");
    EXPECT_EQ(movedTrain.getTotalSeats(), 8);
    EXPECT_EQ(movedTrain.getSeatAllocator()->getAvailableSeatCount(), 7);
}
// Test empty train name
TEST_F(TrainTest, EmptyTrainName) {
    // Should throw because name is empty
    EXPECT_THROW(Train emptyNameTrain(5, "", 10), std::runtime_error);

    // Valid train
    Train validTrain(5, "Valid Name", 10);
    EXPECT_EQ(validTrain.getTrainName(), "Valid Name");

    validTrain.setTrainName("New Name");
    EXPECT_EQ(validTrain.getTrainName(), "New Name");
}

// Test negative train ID
TEST_F(TrainTest, NegativeTrainId) {
    // Should throw because ID is negative
    EXPECT_THROW(Train negativeTrain(-1, "Negative", 10), std::runtime_error);

    // Valid train
    Train validTrain(1, "Positive", 10);
    EXPECT_EQ(validTrain.getTrainId(), 1);

    validTrain.setTrainId(100);
    EXPECT_EQ(validTrain.getTrainId(), 100);
}

// Test zero seats
TEST_F(TrainTest, ZeroSeats) {
    // Should throw if you later uncomment the seat validation
    // EXPECT_THROW(Train zeroSeatsTrain(1, "ZeroSeats", 0), std::runtime_error);

    // Valid train
    Train validTrain(1, "SeatsTest", 5);
    EXPECT_EQ(validTrain.getTotalSeats(), 5);
}


// Test seat allocator persistence
TEST_F(TrainTest, SeatAllocatorPersistence) {
    auto allocator = train1->getSeatAllocator();

    allocator->allocateSeat(101);
    allocator->allocateSeat(102);
    allocator->allocateSeat(103);

    EXPECT_EQ(allocator->getAvailableSeatCount(), 2);

    captureCoutOutput([&]() {
        allocator->freeSeat(1);
    });

    EXPECT_EQ(allocator->getAvailableSeatCount(), 3);
}

// Test getTotalSeats is const
TEST_F(TrainTest, ConstMethods) {
    const Train* constTrain = train1;

    EXPECT_EQ(constTrain->getTrainId(), 1);
    EXPECT_EQ(constTrain->getTrainName(), "Express");
    EXPECT_EQ(constTrain->getTotalSeats(), 5);
    EXPECT_TRUE(constTrain->hasAvailableSeats());
}