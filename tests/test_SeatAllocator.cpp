#include <gtest/gtest.h>
#include "models/SeatAllocator.h"
#include "structures/vector.h"
class SeatAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(SeatAllocatorTest, DefaultConstruction) {
    SeatAllocator allocator;
    EXPECT_EQ(allocator.getTotalSeats(), 10);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 10);
    EXPECT_TRUE(allocator.hasAvailableSeats());
}

TEST_F(SeatAllocatorTest, CustomSeatCount) {
    SeatAllocator allocator(20);
    EXPECT_EQ(allocator.getTotalSeats(), 20);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 20);
}

TEST_F(SeatAllocatorTest, AllocateSingleSeat) {
    SeatAllocator allocator(5);
    int seat = allocator.allocateSeat(101);
    EXPECT_EQ(seat, 1);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 4);
    EXPECT_EQ(allocator.getAllocatedSeatCount(), 1);
}

TEST_F(SeatAllocatorTest, AllocateMultipleSeats) {
    SeatAllocator allocator(5);
    EXPECT_EQ(allocator.allocateSeat(101), 1);
    EXPECT_EQ(allocator.allocateSeat(102), 2);
    EXPECT_EQ(allocator.allocateSeat(103), 3);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 2);
    EXPECT_EQ(allocator.getAllocatedSeatCount(), 3);
}

TEST_F(SeatAllocatorTest, DuplicatePassengerIdThrows) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    EXPECT_THROW(allocator.allocateSeat(101), std::runtime_error);
}

TEST_F(SeatAllocatorTest, DuplicatePassengerInWaitingListThrows) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);
    EXPECT_THROW(allocator.allocateSeat(103), std::runtime_error);
}

TEST_F(SeatAllocatorTest, FullTrainAddsToWaitingList) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    int result = allocator.allocateSeat(103);
    EXPECT_EQ(result, -1);
    EXPECT_FALSE(allocator.getWaitingList().empty());
    EXPECT_FALSE(allocator.hasAvailableSeats());
}

TEST_F(SeatAllocatorTest, FreeSeatValid) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    EXPECT_EQ(allocator.freeSeat(1), 0);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 5);
}

TEST_F(SeatAllocatorTest, FreeSeatInvalidSeatNumberZero) {
    SeatAllocator allocator(5);
    EXPECT_THROW(allocator.freeSeat(0), std::invalid_argument);
}

TEST_F(SeatAllocatorTest, FreeSeatInvalidSeatNumberNegative) {
    SeatAllocator allocator(5);
    EXPECT_THROW(allocator.freeSeat(-1), std::invalid_argument);
}

TEST_F(SeatAllocatorTest, FreeSeatInvalidSeatNumberTooLarge) {
    SeatAllocator allocator(5);
    EXPECT_THROW(allocator.freeSeat(10), std::invalid_argument);
}

TEST_F(SeatAllocatorTest, FreeSeatNotAllocated) {
    SeatAllocator allocator(5);
    EXPECT_THROW(allocator.freeSeat(1), std::out_of_range);
}

TEST_F(SeatAllocatorTest, FreeAlreadyFreedSeat) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    allocator.freeSeat(1);
    EXPECT_THROW(allocator.freeSeat(1), std::out_of_range);
}

TEST_F(SeatAllocatorTest, CancelledSeatReusedBeforeNewSeats) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.freeSeat(1);
    int newSeat = allocator.allocateSeat(103);
    EXPECT_EQ(newSeat, 1);
}

TEST_F(SeatAllocatorTest, MultipleCancelledSeatsReusedLIFO) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);
    allocator.freeSeat(1);
    allocator.freeSeat(2);
    EXPECT_EQ(allocator.allocateSeat(104), 2);
    EXPECT_EQ(allocator.allocateSeat(105), 1);
}

TEST_F(SeatAllocatorTest, AddSeats) {
    SeatAllocator allocator(5);
    allocator.addSeats(5);
    EXPECT_EQ(allocator.getTotalSeats(), 10);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 10);
}

TEST_F(SeatAllocatorTest, AddSeatsWithAllocations) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.addSeats(3);
    EXPECT_EQ(allocator.getTotalSeats(), 8);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 6);
    EXPECT_EQ(allocator.getAllocatedSeatCount(), 2);
}

TEST_F(SeatAllocatorTest, AddInvalidSeatsThrowsZero) {
    SeatAllocator allocator(5);
    EXPECT_THROW(allocator.addSeats(0), std::invalid_argument);
}

TEST_F(SeatAllocatorTest, AddInvalidSeatsThrowsNegative) {
    SeatAllocator allocator(5);
    EXPECT_THROW(allocator.addSeats(-5), std::invalid_argument);
}

TEST_F(SeatAllocatorTest, ChangeTotalSeatsToSameSize) {
    SeatAllocator allocator(10);
    allocator.changeTotalSeats(10);
    EXPECT_EQ(allocator.getTotalSeats(), 10);
}

TEST_F(SeatAllocatorTest, CopyConstructor) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    SeatAllocator copy(allocator);
    EXPECT_EQ(copy.getTotalSeats(), allocator.getTotalSeats());
    EXPECT_EQ(copy.getAvailableSeatCount(), allocator.getAvailableSeatCount());
    EXPECT_EQ(copy.getAllocatedSeatCount(), allocator.getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AssignmentOperator) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    SeatAllocator other(10);
    other = allocator;
    EXPECT_EQ(other.getTotalSeats(), 5);
    EXPECT_EQ(other.getAllocatedSeatCount(), 1);
}

TEST_F(SeatAllocatorTest, CloneMethod) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    auto cloned = allocator.clone();
    EXPECT_EQ(cloned->getTotalSeats(), allocator.getTotalSeats());
    EXPECT_EQ(cloned->getAvailableSeatCount(), allocator.getAvailableSeatCount());
    EXPECT_EQ(cloned->getAllocatedSeatCount(), allocator.getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, FreeSeatReturnsWaitingPassengerId) {
    SeatAllocator allocator(1);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    int waitingPassengerId = allocator.freeSeat(1);
    EXPECT_EQ(waitingPassengerId, 102);
}

TEST_F(SeatAllocatorTest, ProcessWaitingList_Basic) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);
    allocator.allocateSeat(104);

    vector<int> bookedPassengers;
    auto callback = [&bookedPassengers](int passengerId) {
        bookedPassengers.push_back(passengerId);
    };

    allocator.addSeats(2);
    int processed = allocator.processWaitingList(2, callback);

    EXPECT_EQ(processed, 2);
    EXPECT_EQ(bookedPassengers.size(), 2);
    EXPECT_EQ(bookedPassengers[0], 103);
    EXPECT_EQ(bookedPassengers[1], 104);
    EXPECT_EQ(allocator.getWaitingListSize(), 0);
}

TEST_F(SeatAllocatorTest, ProcessWaitingList_MoreSeatsThanWaiting) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    vector<int> bookedPassengers;
    auto callback = [&bookedPassengers](int passengerId) {
        bookedPassengers.push_back(passengerId);
    };

    allocator.addSeats(3);
    int processed = allocator.processWaitingList(3, callback);

    EXPECT_EQ(processed, 1);
    EXPECT_EQ(bookedPassengers.size(), 1);
    EXPECT_EQ(bookedPassengers[0], 103);
    EXPECT_EQ(allocator.getWaitingListSize(), 0);
}

TEST_F(SeatAllocatorTest, ProcessWaitingList_NoWaitingList) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);

    vector<int> bookedPassengers;
    auto callback = [&bookedPassengers](int passengerId) {
        bookedPassengers.push_back(passengerId);
    };

    allocator.addSeats(2);
    int processed = allocator.processWaitingList(2, callback);

    EXPECT_EQ(processed, 0);
    EXPECT_TRUE(bookedPassengers.empty());
}

TEST_F(SeatAllocatorTest, ProcessWaitingList_CallbackThrows) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    auto callback = [](int passengerId) {
        throw std::runtime_error("Booking failed");
    };

    allocator.addSeats(1);
    int processed = allocator.processWaitingList(1, callback);

    EXPECT_EQ(processed, 0);
    EXPECT_EQ(allocator.getWaitingListSize(), 1);
}