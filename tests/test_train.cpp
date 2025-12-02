// ============================================================================
// TrainTest.cpp - Train Model Tests
// ============================================================================

#include <gtest/gtest.h>
#include "models/Train.h"
#include "models/SeatAllocator.h"

class TrainTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// CONSTRUCTION TESTS
// ============================================================================

TEST_F(TrainTest, ValidConstruction) {
    Train train(1, "Express Train", 20);
    EXPECT_EQ(train.getTrainId(), 1);
    EXPECT_EQ(train.getTrainName(), "Express Train");
    EXPECT_EQ(train.getTotalSeats(), 20);
    EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(TrainTest, DefaultSeatCount) {
    Train train(1, "Local Train");
    EXPECT_EQ(train.getTotalSeats(), 10);
}

TEST_F(TrainTest, ConstructionWithMinimumSeats) {
    Train train(1, "Mini Train", 1);
    EXPECT_EQ(train.getTotalSeats(), 1);
}

TEST_F(TrainTest, ConstructionWithLargeCapacity) {
    Train train(1, "Mega Train", 1000);
    EXPECT_EQ(train.getTotalSeats(), 1000);
}

TEST_F(TrainTest, SeatAllocatorCreatedAutomatically) {
    Train train(1, "Express Train", 20);
    SeatAllocator* allocator = train.getSeatAllocator();
    EXPECT_NE(allocator, nullptr);
    EXPECT_EQ(allocator->getTotalSeats(), 20);
}

// ============================================================================
// INVALID CONSTRUCTION TESTS
// ============================================================================

TEST_F(TrainTest, InvalidTrainIdNegative) {
    EXPECT_THROW(Train(-1, "Express Train", 20), std::invalid_argument);
}

TEST_F(TrainTest, InvalidTrainIdZero) {
    Train t(100, "Express Train", 20);
    EXPECT_THROW(t.setTrainId(0), std::runtime_error);
}

TEST_F(TrainTest, EmptyNameThrows) {
    EXPECT_THROW(Train(1, "", 20), std::invalid_argument);
}

TEST_F(TrainTest, NegativeSeatsThrows) {
    EXPECT_THROW(Train(1, "Express Train", -5), std::invalid_argument);
}

TEST_F(TrainTest, AllInvalidParametersThrows) {
    EXPECT_THROW(Train(-1, "", -5), std::invalid_argument);
}

// ============================================================================
// GETTER TESTS
// ============================================================================

TEST_F(TrainTest, GetTrainIdReturnsCorrectValue) {
    Train train(42, "Express Train", 20);
    EXPECT_EQ(train.getTrainId(), 42);
}

TEST_F(TrainTest, GetTrainNameReturnsCorrectValue) {
    Train train(1, "Super Express", 20);
    EXPECT_EQ(train.getTrainName(), "Super Express");
}

TEST_F(TrainTest, GetTotalSeatsReturnsCorrectValue) {
    Train train(1, "Express Train", 50);
    EXPECT_EQ(train.getTotalSeats(), 50);
}

TEST_F(TrainTest, GetSeatAllocatorReturnsValidPointer) {
    Train train(1, "Express Train", 20);
    SeatAllocator* allocator = train.getSeatAllocator();
    EXPECT_NE(allocator, nullptr);
}

// ============================================================================
// SETTER TESTS
// ============================================================================

TEST_F(TrainTest, SetValidTrainName) {
    Train train(1, "Express Train", 20);
    train.setTrainName("Super Express");
    EXPECT_EQ(train.getTrainName(), "Super Express");
}

TEST_F(TrainTest, SetEmptyNameThrows) {
    Train train(1, "Express Train", 20);
    EXPECT_THROW(train.setTrainName(""), std::invalid_argument);
}

TEST_F(TrainTest, SetTrainNameMultipleTimes) {
    Train train(1, "Express Train", 20);
    train.setTrainName("Fast Train");
    train.setTrainName("Ultra Fast Train");
    EXPECT_EQ(train.getTrainName(), "Ultra Fast Train");
}


// ============================================================================
// ADD SEATS TESTS
// ============================================================================

TEST_F(TrainTest, AddSeatsIncreasesTotalSeats) {
    Train train(1, "Express Train", 20);
    train.addSeats(10);
    EXPECT_EQ(train.getTotalSeats(), 30);
}

TEST_F(TrainTest, AddSeatsMultipleTimes) {
    Train train(1, "Express Train", 20);
    train.addSeats(5);
    train.addSeats(5);
    train.addSeats(10);
    EXPECT_EQ(train.getTotalSeats(), 40);
}

TEST_F(TrainTest, AddSeatsUpdatesAllocator) {
    Train train(1, "Express Train", 20);
    train.addSeats(10);

    SeatAllocator* allocator = train.getSeatAllocator();
    EXPECT_EQ(allocator->getTotalSeats(), 30);
}

TEST_F(TrainTest, AddInvalidSeatsZeroThrows) {
    Train train(1, "Express Train", 20);
    EXPECT_THROW(train.addSeats(0), std::invalid_argument);
}

TEST_F(TrainTest, AddInvalidSeatsNegativeThrows) {
    Train train(1, "Express Train", 20);
    EXPECT_THROW(train.addSeats(-5), std::invalid_argument);
}

TEST_F(TrainTest, AddSeatsAfterAllocations) {
    Train train(1, "Express Train", 5);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);

    train.addSeats(5);
    EXPECT_EQ(train.getTotalSeats(), 10);
    EXPECT_TRUE(train.hasAvailableSeats());
}

// ============================================================================
// SET SEATS TESTS
// ============================================================================

TEST_F(TrainTest, SetSeatsIncreasesCapacity) {
    Train train(1, "Express Train", 20);
    train.setSeats(30);
    EXPECT_EQ(train.getTotalSeats(), 30);
}

TEST_F(TrainTest, SetSeatsDecreasesCapacity) {
    Train train(1, "Express Train", 20);
    train.setSeats(15);
    EXPECT_EQ(train.getTotalSeats(), 15);
}

TEST_F(TrainTest, SetSeatsToSameValue) {
    Train train(1, "Express Train", 20);
    train.setSeats(20);
    EXPECT_EQ(train.getTotalSeats(), 20);
}

TEST_F(TrainTest, SetInvalidSeatsZeroThrows) {
    Train train(1, "Express Train", 20);
    EXPECT_THROW(train.setSeats(0), std::invalid_argument);
}

TEST_F(TrainTest, SetInvalidSeatsNegativeThrows) {
    Train train(1, "Express Train", 20);
    EXPECT_THROW(train.setSeats(-10), std::invalid_argument);
}

TEST_F(TrainTest, SetSeatsUpdatesAllocator) {
    Train train(1, "Express Train", 20);
    train.setSeats(25);

    SeatAllocator* allocator = train.getSeatAllocator();
    EXPECT_EQ(allocator->getTotalSeats(), 25);
}

TEST_F(TrainTest, SetSeatsBelowAllocatedThrows) {
    Train train(1, "Express Train", 10);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);
    train.getSeatAllocator()->allocateSeat(103);

    EXPECT_THROW(train.setSeats(2), std::out_of_range);
}

// ============================================================================
// AVAILABLE SEATS TESTS
// ============================================================================

TEST_F(TrainTest, HasAvailableSeatsInitially) {
    Train train(1, "Express Train", 20);
    EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(TrainTest, HasAvailableSeatsAfterPartialAllocation) {
    Train train(1, "Express Train", 5);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);
    EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(TrainTest, NoAvailableSeatsWhenFull) {
    Train train(1, "Express Train", 2);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);
    EXPECT_FALSE(train.hasAvailableSeats());
}

TEST_F(TrainTest, HasAvailableSeatsAfterFreeing) {
    Train train(1, "Express Train", 2);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);
    train.getSeatAllocator()->freeSeat(1);
    EXPECT_TRUE(train.hasAvailableSeats());
}

// ============================================================================
// COPY CONSTRUCTOR TESTS
// ============================================================================

TEST_F(TrainTest, CopyConstructorCopiesAllFields) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);

    Train copy(train);
    EXPECT_EQ(copy.getTrainId(), train.getTrainId());
    EXPECT_EQ(copy.getTrainName(), train.getTrainName());
    EXPECT_EQ(copy.getTotalSeats(), train.getTotalSeats());
}

TEST_F(TrainTest, CopyConstructorCreatesIndependentCopy) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);

    Train copy(train);
    copy.getSeatAllocator()->allocateSeat(102);

    EXPECT_NE(train.getSeatAllocator(), copy.getSeatAllocator());
}

TEST_F(TrainTest, CopyConstructorCopiesSeatAllocator) {
    Train train(1, "Express Train", 5);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);

    Train copy(train);
    SeatAllocator* origAllocator = train.getSeatAllocator();
    SeatAllocator* copyAllocator = copy.getSeatAllocator();

    EXPECT_EQ(origAllocator->getAllocatedSeatCount(),
              copyAllocator->getAllocatedSeatCount());
}

// ============================================================================
// ASSIGNMENT OPERATOR TESTS
// ============================================================================

TEST_F(TrainTest, AssignmentOperatorCopiesAllFields) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);

    Train other(2, "Local Train", 10);
    other = train;

    EXPECT_EQ(other.getTrainId(), 1);
    EXPECT_EQ(other.getTrainName(), "Express Train");
    EXPECT_EQ(other.getTotalSeats(), 20);
}

TEST_F(TrainTest, SelfAssignmentHandled) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);

    train = train;

    EXPECT_EQ(train.getTrainId(), 1);
    EXPECT_EQ(train.getTrainName(), "Express Train");
    EXPECT_EQ(train.getTotalSeats(), 20);
}

TEST_F(TrainTest, AssignmentOperatorCreatesIndependentCopy) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);

    Train other(2, "Local Train", 10);
    other = train;

    other.getSeatAllocator()->allocateSeat(102);

    EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 1);
    EXPECT_EQ(other.getSeatAllocator()->getAllocatedSeatCount(), 2);
}

// ============================================================================
// CLONE METHOD TESTS
// ============================================================================

TEST_F(TrainTest, CloneCreatesExactCopy) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);

    auto cloned = train.clone();
    EXPECT_EQ(cloned->getTrainId(), train.getTrainId());
    EXPECT_EQ(cloned->getTrainName(), train.getTrainName());
    EXPECT_EQ(cloned->getTotalSeats(), train.getTotalSeats());
}

TEST_F(TrainTest, CloneCreatesIndependentObject) {
    Train train(1, "Express Train", 20);
    train.getSeatAllocator()->allocateSeat(101);

    auto cloned = train.clone();
    cloned->getSeatAllocator()->allocateSeat(102);

    EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 1);
    EXPECT_EQ(cloned->getSeatAllocator()->getAllocatedSeatCount(), 2);
}

TEST_F(TrainTest, ClonePreservesSeatAllocatorState) {
    Train train(1, "Express Train", 5);
    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);
    train.getSeatAllocator()->allocateSeat(103);

    auto cloned = train.clone();

    EXPECT_EQ(cloned->getSeatAllocator()->getAllocatedSeatCount(), 3);
    EXPECT_EQ(cloned->getSeatAllocator()->getAvailableSeatCount(), 2);
}

// ============================================================================
// INTEGRATION WITH SEAT ALLOCATOR TESTS
// ============================================================================

TEST_F(TrainTest, AllocateSeatThroughTrain) {
    Train train(1, "Express Train", 5);
    SeatAllocator* allocator = train.getSeatAllocator();

    int seat = allocator->allocateSeat(101);
    EXPECT_EQ(seat, 1);
    EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(TrainTest, FillTrainCompletely) {
    Train train(1, "Express Train", 3);
    SeatAllocator* allocator = train.getSeatAllocator();

    allocator->allocateSeat(101);
    allocator->allocateSeat(102);
    allocator->allocateSeat(103);

    EXPECT_FALSE(train.hasAvailableSeats());
}

TEST_F(TrainTest, AddSeatsAfterFull) {
    Train train(1, "Express Train", 2);
    SeatAllocator* allocator = train.getSeatAllocator();

    allocator->allocateSeat(101);
    allocator->allocateSeat(102);
    EXPECT_FALSE(train.hasAvailableSeats());

    train.addSeats(3);
    EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(TrainTest, FreeSeatRestoresAvailability) {
    Train train(1, "Express Train", 2);
    SeatAllocator* allocator = train.getSeatAllocator();

    allocator->allocateSeat(101);
    allocator->allocateSeat(102);
    allocator->freeSeat(1);

    EXPECT_TRUE(train.hasAvailableSeats());
}

// ============================================================================
// EDGE CASE TESTS
// ============================================================================

TEST_F(TrainTest, SingleSeatTrain) {
    Train train(1, "Mini Train", 1);
    train.getSeatAllocator()->allocateSeat(101);
    EXPECT_FALSE(train.hasAvailableSeats());
}

TEST_F(TrainTest, LargeCapacityTrain) {
    Train train(1, "Mega Train", 10000);
    EXPECT_EQ(train.getTotalSeats(), 10000);
    EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(TrainTest, TrainNameWithNumbers) {
    Train train(1, "Train123", 20);
    EXPECT_EQ(train.getTrainName(), "Train123");
}

TEST_F(TrainTest, TrainNameWithSpaces) {
    Train train(1, "High Speed Rail", 20);
    EXPECT_EQ(train.getTrainName(), "High Speed Rail");
}

TEST_F(TrainTest, MaximumTrainId) {
    Train train(999999, "Express Train", 20);
    EXPECT_EQ(train.getTrainId(), 999999);
}

// ============================================================================
// STRESS TESTS
// ============================================================================

TEST_F(TrainTest, AllocateAndFreeManySeats) {
    Train train(1, "Express Train", 100);
    SeatAllocator* allocator = train.getSeatAllocator();

    // Allocate 50 seats
    for (int i = 1; i <= 50; i++) {
        allocator->allocateSeat(100 + i);
    }

    // Free first 25 seats
    for (int i = 1; i <= 25; i++) {
        allocator->freeSeat(i);
    }

    EXPECT_TRUE(train.hasAvailableSeats());
    EXPECT_EQ(allocator->getAllocatedSeatCount(), 25);
}

TEST_F(TrainTest, MultipleOperationsSequence) {
    Train train(1, "Express Train", 10);

    train.addSeats(5);
    EXPECT_EQ(train.getTotalSeats(), 15);

    train.setSeats(20);
    EXPECT_EQ(train.getTotalSeats(), 20);

    train.getSeatAllocator()->allocateSeat(101);
    EXPECT_TRUE(train.hasAvailableSeats());

    train.setTrainName("Updated Express");
    EXPECT_EQ(train.getTrainName(), "Updated Express");
}

