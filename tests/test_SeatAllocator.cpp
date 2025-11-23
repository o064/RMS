#include <gtest/gtest.h>
#include "models/SeatAllocator.h"
#include <sstream>
#include <iostream>
#include <functional>

class SeatAllocatorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}

    // Helper to capture std::cout output
    std::string captureCoutOutput(std::function<void()> func) {
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
        func();
        std::cout.rdbuf(old);
        return buffer.str();
    }
};

// ---------------------- Constructor Tests ----------------------
TEST_F(SeatAllocatorTest, ConstructorMinimumSeats) {
    SeatAllocator allocator(0);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 10); // default 10 if seat is less than 0

    SeatAllocator allocator2(15);
    EXPECT_EQ(allocator2.getAvailableSeatCount(), 15);
}

TEST_F(SeatAllocatorTest, ConstructorNegativeSeats) {
    SeatAllocator allocator(-5);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 10); // should default to 10
}

// ---------------------- Allocation Tests ----------------------
TEST_F(SeatAllocatorTest, AllocateSeats) {
    SeatAllocator allocator(3);
    int seat1 = allocator.allocateSeat(101);
    int seat2 = allocator.allocateSeat(102);
    int seat3 = allocator.allocateSeat(103);

    EXPECT_EQ(seat1, 1);
    EXPECT_EQ(seat2, 2);
    EXPECT_EQ(seat3, 3);
    EXPECT_FALSE(allocator.hasAvailableSeats());
    EXPECT_EQ(allocator.getAvailableSeatCount(), 0);
}

TEST_F(SeatAllocatorTest, AllocateWhenFullGoesToWaitingList) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);

    std::string output = captureCoutOutput([&]() {
        int seat = allocator.allocateSeat(103);
        EXPECT_EQ(seat, -1);
    });

    EXPECT_TRUE(output.find("Train full, passenger added to waiting list") != std::string::npos);
}

// ---------------------- Duplicate Passenger ID ----------------------
TEST_F(SeatAllocatorTest, DuplicatePassengerID_InAllocatedSeats) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);

    std::string out1 = captureCoutOutput([&]() {
        int res = allocator.allocateSeat(101); // duplicate in allocatedSeats
        EXPECT_EQ(res, -1);
    });
    EXPECT_TRUE(out1.find("Passenger ID 101 is already allocated a seat") != std::string::npos);
}

TEST_F(SeatAllocatorTest, DuplicatePassengerID_InWaitingList) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);

    // First addition to waiting list should succeed
    std::string out1 = captureCoutOutput([&]() {
        int res = allocator.allocateSeat(103);
        EXPECT_EQ(res, -1);
    });
    EXPECT_TRUE(out1.find("Train full, passenger added to waiting list") != std::string::npos);

    // Second addition of same passenger should fail
    std::string out2 = captureCoutOutput([&]() {
        int res = allocator.allocateSeat(103);
        EXPECT_EQ(res, -1);
    });
    EXPECT_TRUE(out2.find("the passenger is already in the waiting list") != std::string::npos);
}

// ---------------------- Free Seat Tests ----------------------
TEST_F(SeatAllocatorTest, FreeSeatBasic) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);

    std::string output = captureCoutOutput([&]() {
        allocator.freeSeat(1);
    });

    EXPECT_EQ(allocator.getAvailableSeatCount(), 2); // seat 1 freed + seat 3 available
}

TEST_F(SeatAllocatorTest, FreeInvalidSeat_Zero) {
    SeatAllocator allocator(5);
    std::string out = captureCoutOutput([&]() {
        allocator.freeSeat(0);
    });
    EXPECT_TRUE(out.find("this seat not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeInvalidSeat_TooHigh) {
    SeatAllocator allocator(5);
    std::string out = captureCoutOutput([&]() {
        allocator.freeSeat(10);
    });
    EXPECT_TRUE(out.find("this seat not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeInvalidSeat_Negative) {
    SeatAllocator allocator(5);
    std::string out = captureCoutOutput([&]() {
        allocator.freeSeat(-1);
    });
    EXPECT_TRUE(out.find("this seat not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeUnallocatedSeat) {
    SeatAllocator allocator(5);
    std::string out = captureCoutOutput([&]() {
        allocator.freeSeat(3);
    });
    EXPECT_TRUE(out.find("this seat not yet allocated") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeAlreadyFreedSeat) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);

    captureCoutOutput([&]() {
        allocator.freeSeat(1);
    });

    // Try to free the same seat again
    std::string out = captureCoutOutput([&]() {
        allocator.freeSeat(1);
    });
    EXPECT_TRUE(out.find("this seat not yet allocated") != std::string::npos);
}

// ---------------------- Waiting List Tests ----------------------
TEST_F(SeatAllocatorTest, FreeSeatAssignsFromWaitingList) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);

    // Add to waiting list
    captureCoutOutput([&]() {
        allocator.allocateSeat(103);
    });

    std::string output = captureCoutOutput([&]() {
        allocator.freeSeat(1); // seat 1 should go to passenger 103
    });

    EXPECT_TRUE(output.find("seat 1 is assigned to passenger with id :103") != std::string::npos);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 0); // no seats available, all allocated
}

TEST_F(SeatAllocatorTest, WaitingListFIFO) {
    SeatAllocator allocator(1);
    allocator.allocateSeat(101);

    // Add multiple passengers to waiting list
    captureCoutOutput([&]() {
        allocator.allocateSeat(102);
        allocator.allocateSeat(103);
        allocator.allocateSeat(104);
    });

    // Free seat - should go to first in waiting list (102)
    std::string output = captureCoutOutput([&]() {
        allocator.freeSeat(1);
    });
    EXPECT_TRUE(output.find("passenger with id :102") != std::string::npos);
}

// ---------------------- Cancelled Seats / LIFO ----------------------
TEST_F(SeatAllocatorTest, AllocateAfterFreeUsesCancelledStack) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    captureCoutOutput([&]() {
        allocator.freeSeat(2);
    }); // push seat 2 to cancelledSeats

    int newSeat = allocator.allocateSeat(104);
    EXPECT_EQ(newSeat, 2); // should reuse seat 2 (LIFO)
}

TEST_F(SeatAllocatorTest, CancelledSeatsLIFO) {
    SeatAllocator allocator(5);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    // Free seats in order: 1, 2, 3
    captureCoutOutput([&]() {
        allocator.freeSeat(1);
        allocator.freeSeat(2);
        allocator.freeSeat(3);
    });

    // Allocate new passengers - should get seats in LIFO order: 3, 2, 1
    int seat1 = allocator.allocateSeat(201);
    int seat2 = allocator.allocateSeat(202);
    int seat3 = allocator.allocateSeat(203);

    EXPECT_EQ(seat1, 3);
    EXPECT_EQ(seat2, 2);
    EXPECT_EQ(seat3, 1);
}

// ---------------------- hasAvailableSeats / getAvailableSeatCount ----------------------
TEST_F(SeatAllocatorTest, HasAvailableSeatsAndCount) {
    SeatAllocator allocator(2);
    EXPECT_TRUE(allocator.hasAvailableSeats());
    EXPECT_EQ(allocator.getAvailableSeatCount(), 2);

    allocator.allocateSeat(101);
    EXPECT_TRUE(allocator.hasAvailableSeats());
    EXPECT_EQ(allocator.getAvailableSeatCount(), 1);

    allocator.allocateSeat(102);
    EXPECT_FALSE(allocator.hasAvailableSeats());
    EXPECT_EQ(allocator.getAvailableSeatCount(), 0);
}

TEST_F(SeatAllocatorTest, AvailableSeatsCountIncludesCancelled) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    EXPECT_EQ(allocator.getAvailableSeatCount(), 0);

    captureCoutOutput([&]() {
        allocator.freeSeat(1);
    });

    EXPECT_EQ(allocator.getAvailableSeatCount(), 1);
    EXPECT_TRUE(allocator.hasAvailableSeats());
}

// ---------------------- printStatus Tests ----------------------
TEST_F(SeatAllocatorTest, PrintStatusOutput) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);

    std::string out = captureCoutOutput([&]() {
        allocator.printStatus();
    });

    EXPECT_TRUE(out.find("Total Seats: 2") != std::string::npos);
    EXPECT_TRUE(out.find("Available Seats Count:") != std::string::npos);
    EXPECT_TRUE(out.find("Seat 1->Passenger 101") != std::string::npos);
}

TEST_F(SeatAllocatorTest, PrintStatusEmpty) {
    SeatAllocator allocator(3);

    std::string out = captureCoutOutput([&]() {
        allocator.printStatus();
    });

    EXPECT_TRUE(out.find("Total Seats: 3") != std::string::npos);
    EXPECT_TRUE(out.find("Available Seats Count: 3") != std::string::npos);
    EXPECT_TRUE(out.find("Allocated Seats: None") != std::string::npos);
}

// ---------------------- Edge Case Tests ----------------------
TEST_F(SeatAllocatorTest, SingleSeatAllocator) {
    SeatAllocator allocator(1);
    EXPECT_TRUE(allocator.hasAvailableSeats());

    int seat = allocator.allocateSeat(101);
    EXPECT_EQ(seat, 1);
    EXPECT_FALSE(allocator.hasAvailableSeats());

    std::string out = captureCoutOutput([&]() {
        int res = allocator.allocateSeat(102);
        EXPECT_EQ(res, -1);
    });
    EXPECT_TRUE(out.find("Train full, passenger added to waiting list") != std::string::npos);
}

// ---------------------- Copy and Assignment Tests ----------------------
TEST_F(SeatAllocatorTest, CopyConstructor) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);

    SeatAllocator copied(allocator);

    EXPECT_EQ(copied.getAvailableSeatCount(), allocator.getAvailableSeatCount());
    EXPECT_EQ(copied.hasAvailableSeats(), allocator.hasAvailableSeats());
}

TEST_F(SeatAllocatorTest, AssignmentOperator) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);

    SeatAllocator assigned(5);
    assigned = allocator;

    EXPECT_EQ(assigned.getAvailableSeatCount(), allocator.getAvailableSeatCount());
}

TEST_F(SeatAllocatorTest, CloneMethod) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);

    auto cloned = allocator.clone();

    EXPECT_EQ(cloned->getAvailableSeatCount(), allocator.getAvailableSeatCount());
}

// ---------------------- Complex Scenarios ----------------------
TEST_F(SeatAllocatorTest, ComplexAllocationScenario) {
    SeatAllocator allocator(3);

    // Fill all seats
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    // Add to waiting list
    captureCoutOutput([&]() {
        allocator.allocateSeat(104);
        allocator.allocateSeat(105);
    });

    // Free a seat - should go to 104
    std::string out1 = captureCoutOutput([&]() {
        allocator.freeSeat(2);
    });
    EXPECT_TRUE(out1.find("passenger with id :104") != std::string::npos);

    // Free another seat - should go to 105
    std::string out2 = captureCoutOutput([&]() {
        allocator.freeSeat(1);
    });
    EXPECT_TRUE(out2.find("passenger with id :105") != std::string::npos);

    // Now free a seat with empty waiting list
    captureCoutOutput([&]() {
        allocator.freeSeat(3);
    });

    // Allocate new passenger - should get seat 3
    int newSeat = allocator.allocateSeat(106);
    EXPECT_EQ(newSeat, 3);
}