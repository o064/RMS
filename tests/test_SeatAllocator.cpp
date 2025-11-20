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
    EXPECT_EQ(allocator.getAvailableSeatCount(), 10); // default  10 if seat is less than 0

    SeatAllocator allocator2(15);
    EXPECT_EQ(allocator2.getAvailableSeatCount(), 15);
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
TEST_F(SeatAllocatorTest, DuplicatePassengerID_Prevented) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);

    std::string out1 = captureCoutOutput([&]() {
        int res = allocator.allocateSeat(101); // duplicate in allocatedSeats
        EXPECT_EQ(res, -1);
    });
    EXPECT_TRUE(out1.find("Passenger ID 101 is already allocated a seat") != std::string::npos);

    allocator.allocateSeat(102);
    std::string out2 = captureCoutOutput([&]() {
        int res = allocator.allocateSeat(102); // duplicate in waitingList
        EXPECT_EQ(res, -1);
    });
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

TEST_F(SeatAllocatorTest, FreeInvalidSeat) {
    SeatAllocator allocator(5);
    std::string out1 = captureCoutOutput([&]() { allocator.freeSeat(0); });
    EXPECT_TRUE(out1.find("this seat not exist") != std::string::npos);

    std::string out2 = captureCoutOutput([&]() { allocator.freeSeat(10); });
    EXPECT_TRUE(out2.find("this seat not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeUnallocatedSeat) {
    SeatAllocator allocator(5);
    std::string out = captureCoutOutput([&]() { allocator.freeSeat(3); });
    EXPECT_TRUE(out.find("this seat not yet allocated") != std::string::npos);
}

// ---------------------- Waiting List Tests ----------------------
TEST_F(SeatAllocatorTest, FreeSeatAssignsFromWaitingList) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);

    // Add to waiting list
    allocator.allocateSeat(103);

    std::string output = captureCoutOutput([&]() {
        allocator.freeSeat(1); // seat 1 should go to passenger 103
    });

    EXPECT_TRUE(output.find("seat 1 is assigned to passenger with id :103") != std::string::npos);
    EXPECT_EQ(allocator.getAvailableSeatCount(), 0); // no seats available
}

// ---------------------- Cancelled Seats / LIFO ----------------------
TEST_F(SeatAllocatorTest, AllocateAfterFreeUsesCancelledStack) {
    SeatAllocator allocator(3);
    allocator.allocateSeat(101);
    allocator.allocateSeat(102);
    allocator.allocateSeat(103);

    captureCoutOutput([&]() { allocator.freeSeat(2); }); // push seat 2 to cancelledSeats

    int newSeat = allocator.allocateSeat(104);
    EXPECT_EQ(newSeat, 2); // should reuse seat 2 (LIFO)
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

// ---------------------- printStatus Tests ----------------------
TEST_F(SeatAllocatorTest, PrintStatusOutput) {
    SeatAllocator allocator(2);
    allocator.allocateSeat(101);

    std::string out = captureCoutOutput([&]() { allocator.printStatus(); });
    EXPECT_TRUE(out.find("Total Seats: 2") != std::string::npos);
    EXPECT_TRUE(out.find("Available Seats Count:") != std::string::npos);
    EXPECT_TRUE(out.find("Allocated Seats: Seat 1->Passenger 101") != std::string::npos);
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

// ---------------------- Test that duplicate passenger IDs are not allowed ----------------------

TEST_F(SeatAllocatorTest, DuplicatePassengerIDs) {
    SeatAllocator allocator(2);

    // Allocate seat for passenger 101
    int seat1 = allocator.allocateSeat(101);
    EXPECT_EQ(seat1, 1);

    // Try allocating the same passenger again
    std::string output = captureCoutOutput([&]() {
        int seatDuplicate = allocator.allocateSeat(101);
        EXPECT_EQ(seatDuplicate, -1);
    });
    EXPECT_TRUE(output.find("Passenger ID 101 is already allocated a seat.") != std::string::npos);

    // Allocate a new passenger normally
    int seat2 = allocator.allocateSeat(102);
    EXPECT_EQ(seat2, 2);

    // Try adding same passenger to waiting list
    allocator.allocateSeat(103); // goes to waiting list
    std::string outputWait = captureCoutOutput([&]() {
        int seatDuplicateWait = allocator.allocateSeat(103);
        EXPECT_EQ(seatDuplicateWait, -1);
    });
    EXPECT_TRUE(outputWait.find("the passenger is already in the waiting list") != std::string::npos);
}
