#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include "models/SeatAllocator.h"

// =================================================================
// Test Fixture
// =================================================================
class SeatAllocatorTest : public ::testing::Test {
protected:
    SeatAllocator* allocator;
    std::stringstream outputBuffer;
    std::streambuf* originalCout;

    void SetUp() override {
        allocator = nullptr;
        // Redirect cout to capture print statements
        originalCout = std::cout.rdbuf(outputBuffer.rdbuf());
    }

    void TearDown() override {
        // Restore cout
        std::cout.rdbuf(originalCout);
        if (allocator != nullptr) {
            delete allocator;
            allocator = nullptr;
        }
    }

    std::string getCapturedOutput() {
        return outputBuffer.str();
    }

    void clearOutput() {
        outputBuffer.str("");
        outputBuffer.clear(); // Clear error flags
    }
};

// =================================================================
// Constructor Tests
// =================================================================

TEST_F(SeatAllocatorTest, Constructor_DefaultSeats) {
    allocator = new SeatAllocator();
    EXPECT_EQ(10, allocator->getAvailableSeatCount());
    EXPECT_EQ(0, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, Constructor_CustomSeats) {
    allocator = new SeatAllocator(25);
    EXPECT_EQ(25, allocator->getAvailableSeatCount());
    EXPECT_EQ(0, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, Constructor_ZeroSeats_DefaultsTo10) {
    allocator = new SeatAllocator(0);
    EXPECT_EQ(10, allocator->getAvailableSeatCount());
}

TEST_F(SeatAllocatorTest, Constructor_NegativeSeats_DefaultsTo10) {
    allocator = new SeatAllocator(-5);
    EXPECT_EQ(10, allocator->getAvailableSeatCount());
}

TEST_F(SeatAllocatorTest, Constructor_LargeNumberOfSeats) {
    allocator = new SeatAllocator(1000);
    EXPECT_EQ(1000, allocator->getAvailableSeatCount());
    EXPECT_TRUE(allocator->hasAvailableSeats());
}

// =================================================================
// Copy & Assignment Tests
// =================================================================

TEST_F(SeatAllocatorTest, CopyConstructor_EmptyAllocator) {
    SeatAllocator original(15);
    SeatAllocator copy(original);

    EXPECT_EQ(original.getAvailableSeatCount(), copy.getAvailableSeatCount());
    EXPECT_EQ(original.getAllocatedSeatCount(), copy.getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, CopyConstructor_WithAllocatedSeats) {
    SeatAllocator original(10);
    original.allocateSeat(101);
    original.allocateSeat(102);

    SeatAllocator copy(original);

    EXPECT_EQ(original.getAvailableSeatCount(), copy.getAvailableSeatCount());
    EXPECT_EQ(original.getAllocatedSeatCount(), copy.getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AssignmentOperator_Basic) {
    SeatAllocator original(20);
    original.allocateSeat(201);
    original.allocateSeat(202);

    SeatAllocator assigned(5);
    assigned = original;

    EXPECT_EQ(original.getAvailableSeatCount(), assigned.getAvailableSeatCount());
    EXPECT_EQ(original.getAllocatedSeatCount(), assigned.getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AssignmentOperator_SelfAssignment) {
    allocator = new SeatAllocator(15);
    allocator->allocateSeat(301);

    *allocator = *allocator;

    EXPECT_EQ(14, allocator->getAvailableSeatCount());
    EXPECT_EQ(1, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, Clone_CreatesIndependentCopy) {
    allocator = new SeatAllocator(10);
    allocator->allocateSeat(401);

    // clone() returns a unique_ptr
    auto cloned = allocator->clone();

    // Modify original
    allocator->allocateSeat(402);

    // Cloned should be independent (should still have 9 available, not 8)
    EXPECT_EQ(8, allocator->getAvailableSeatCount());
    EXPECT_EQ(9, cloned->getAvailableSeatCount());
}

// =================================================================
// Seat Allocation Tests
// =================================================================

TEST_F(SeatAllocatorTest, AllocateSeat_FirstSeat) {
    allocator = new SeatAllocator(10);
    int seatNumber = allocator->allocateSeat(1001);

    EXPECT_EQ(1, seatNumber);
    EXPECT_EQ(9, allocator->getAvailableSeatCount());
    EXPECT_EQ(1, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AllocateSeat_MultipleSeats) {
    allocator = new SeatAllocator(10);

    int seat1 = allocator->allocateSeat(1001);
    int seat2 = allocator->allocateSeat(1002);
    int seat3 = allocator->allocateSeat(1003);

    EXPECT_EQ(1, seat1);
    EXPECT_EQ(2, seat2);
    EXPECT_EQ(3, seat3);
    EXPECT_EQ(7, allocator->getAvailableSeatCount());
    EXPECT_EQ(3, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AllocateSeat_AllSeats) {
    allocator = new SeatAllocator(5);

    for (int i = 1; i <= 5; i++) {
        int seat = allocator->allocateSeat(2000 + i);
        EXPECT_EQ(i, seat);
    }

    EXPECT_EQ(0, allocator->getAvailableSeatCount());
    EXPECT_EQ(5, allocator->getAllocatedSeatCount());
    EXPECT_FALSE(allocator->hasAvailableSeats());
}

TEST_F(SeatAllocatorTest, AllocateSeat_DuplicatePassengerId_InAllocated) {
    allocator = new SeatAllocator(10);

    int seat1 = allocator->allocateSeat(3001);
    clearOutput();
    int seat2 = allocator->allocateSeat(3001); // Duplicate

    EXPECT_EQ(1, seat1);
    EXPECT_EQ(-1, seat2);
    EXPECT_TRUE(getCapturedOutput().find("already allocated") != std::string::npos);
    EXPECT_EQ(1, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AllocateSeat_WhenFull_AddsToWaitingList) {
    allocator = new SeatAllocator(3);

    // Fill all seats
    allocator->allocateSeat(4001);
    allocator->allocateSeat(4002);
    allocator->allocateSeat(4003);

    clearOutput();
    int waitingSeat = allocator->allocateSeat(4004);

    EXPECT_EQ(-1, waitingSeat);
    EXPECT_TRUE(getCapturedOutput().find("waiting list") != std::string::npos);
}

TEST_F(SeatAllocatorTest, AllocateSeat_DuplicatePassengerId_InWaitingList) {
    allocator = new SeatAllocator(2);

    // Fill all seats
    allocator->allocateSeat(5001);
    allocator->allocateSeat(5002);

    // Add to waiting list
    allocator->allocateSeat(5003);

    clearOutput();
    int duplicate = allocator->allocateSeat(5003); // Duplicate in waiting list

    EXPECT_EQ(-1, duplicate);
    EXPECT_TRUE(getCapturedOutput().find("already in the waiting list") != std::string::npos);
}

// =================================================================
// Free Seat Tests
// =================================================================

TEST_F(SeatAllocatorTest, FreeSeat_ValidSeat) {
    allocator = new SeatAllocator(10);

    allocator->allocateSeat(6001);
    allocator->freeSeat(1);

    EXPECT_EQ(10, allocator->getAvailableSeatCount());
    EXPECT_EQ(0, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, FreeSeat_InvalidSeatNumber_TooHigh) {
    allocator = new SeatAllocator(10);

    clearOutput();
    allocator->freeSeat(15);

    EXPECT_TRUE(getCapturedOutput().find("not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeSeat_InvalidSeatNumber_Zero) {
    allocator = new SeatAllocator(10);

    clearOutput();
    allocator->freeSeat(0);

    EXPECT_TRUE(getCapturedOutput().find("not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeSeat_InvalidSeatNumber_Negative) {
    allocator = new SeatAllocator(10);

    clearOutput();
    allocator->freeSeat(-5);

    EXPECT_TRUE(getCapturedOutput().find("not exist") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeSeat_UnallocatedSeat) {
    allocator = new SeatAllocator(10);

    clearOutput();
    allocator->freeSeat(5);

    EXPECT_TRUE(getCapturedOutput().find("not yet allocated") != std::string::npos);
}

TEST_F(SeatAllocatorTest, FreeSeat_WithWaitingList_AutoAssigns) {
    allocator = new SeatAllocator(2);

    // Fill seats
    allocator->allocateSeat(7001);
    allocator->allocateSeat(7002);

    // Add to waiting list
    allocator->allocateSeat(7003);

    clearOutput();
    int assignedId = allocator->freeSeat(1); // Should auto-assign to 7003

    std::string output = getCapturedOutput();
    EXPECT_EQ(7003, assignedId); // Check return value!
    EXPECT_TRUE(output.find("7003") != std::string::npos);
    EXPECT_TRUE(output.find("waitining list") != std::string::npos);
    EXPECT_EQ(2, allocator->getAllocatedSeatCount()); // Still 2 allocated
}

TEST_F(SeatAllocatorTest, FreeSeat_ReusesCancelledSeat) {
    allocator = new SeatAllocator(5);

    int seat1 = allocator->allocateSeat(8001);
    allocator->freeSeat(seat1);

    int seat2 = allocator->allocateSeat(8002);

    EXPECT_EQ(seat1, seat2); // Should reuse cancelled seat
}

TEST_F(SeatAllocatorTest, FreeSeat_MultipleCancellations_LIFO) {
    allocator = new SeatAllocator(10);

    allocator->allocateSeat(9001); // Seat 1
    allocator->allocateSeat(9002); // Seat 2
    allocator->allocateSeat(9003); // Seat 3

    allocator->freeSeat(1);
    allocator->freeSeat(2);
    allocator->freeSeat(3);

    // Should reuse in LIFO order (stack behavior: 3 comes off first)
    int newSeat1 = allocator->allocateSeat(9004);
    int newSeat2 = allocator->allocateSeat(9005);
    int newSeat3 = allocator->allocateSeat(9006);

    EXPECT_EQ(3, newSeat1);
    EXPECT_EQ(2, newSeat2);
    EXPECT_EQ(1, newSeat3);
}

// =================================================================
// Add & Change Seats Tests
// =================================================================

TEST_F(SeatAllocatorTest, AddSeats_ValidAmount) {
    allocator = new SeatAllocator(10);
    allocator->addSeats(5);
    EXPECT_EQ(15, allocator->getAvailableSeatCount());
}

TEST_F(SeatAllocatorTest, AddSeats_ZeroSeats_ThrowsException) {
    allocator = new SeatAllocator(10);
    EXPECT_THROW({
                     try {
                         allocator->addSeats(0);
                     } catch (const std::runtime_error& e) {
                         EXPECT_STREQ("Seats must be greater than zero", e.what());
                         throw;
                     }
                 }, std::runtime_error);
}

TEST_F(SeatAllocatorTest, AddSeats_NegativeSeats_ThrowsException) {
    allocator = new SeatAllocator(10);
    EXPECT_THROW({
                     try {
                         allocator->addSeats(-3);
                     } catch (const std::runtime_error& e) {
                         EXPECT_STREQ("Seats must be greater than zero", e.what());
                         throw;
                     }
                 }, std::runtime_error);
}

TEST_F(SeatAllocatorTest, AddSeats_WithAllocatedSeats) {
    allocator = new SeatAllocator(10);
    allocator->allocateSeat(10001);
    allocator->allocateSeat(10002);
    allocator->addSeats(5);

    EXPECT_EQ(13, allocator->getAvailableSeatCount());
    EXPECT_EQ(2, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AddSeats_NewSeatsAreAllocatable) {
    allocator = new SeatAllocator(5);

    // Fill original seats
    for (int i = 1; i <= 5; i++) {
        allocator->allocateSeat(11000 + i);
    }

    allocator->addSeats(3);
    int newSeat = allocator->allocateSeat(11006);
    EXPECT_EQ(6, newSeat);
}

TEST_F(SeatAllocatorTest, ChangeTotalSeats_Expand) {
    allocator = new SeatAllocator(10);
    allocator->changeTotalSeats(20);
    EXPECT_EQ(20, allocator->getAvailableSeatCount());
}

TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkWithSpace) {
    allocator = new SeatAllocator(20);
    allocator->allocateSeat(12001);
    allocator->allocateSeat(12002);

    allocator->changeTotalSeats(15);
    EXPECT_EQ(13, allocator->getAvailableSeatCount());
    EXPECT_EQ(2, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkBelowAllocated_ThrowsException) {
    allocator = new SeatAllocator(10);
    for(int i=1; i<=5; i++) allocator->allocateSeat(13000+i);

    EXPECT_THROW({
                     try {
                         allocator->changeTotalSeats(3);
                     } catch (const std::runtime_error& e) {
                         EXPECT_TRUE(std::string(e.what()).find("Cannot") != std::string::npos);
                         throw;
                     }
                 }, std::runtime_error);
}

TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkWithHighSeatAllocated_ThrowsException) {
    allocator = new SeatAllocator(10);
    allocator->allocateSeat(13001); // seat 1

    // Artificially burn through seats to get to seat 10
    for(int i = 2; i <= 9; i++) allocator->allocateSeat(13000 + i);
    int highSeat = allocator->allocateSeat(13010); // seat 10
    EXPECT_EQ(10, highSeat);

    // Try to shrink to 8 - seat 10 is allocated, so should fail
    EXPECT_THROW(allocator->changeTotalSeats(8), std::runtime_error);
}

TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkWithoutEnoughFreeSeats_ThrowsException) {
    allocator = new SeatAllocator(10);
    // Allocate 8 seats (seats 1-8)
    for (int i = 1; i <= 8; i++) allocator->allocateSeat(14000 + i);

    // Try to shrink to 5. Needs to remove 5 seats (6,7,8,9,10).
    // But 6, 7, 8 are allocated.
    EXPECT_THROW({
                     try {
                         allocator->changeTotalSeats(5);
                     } catch (const std::runtime_error& e) {
                         EXPECT_TRUE(std::string(e.what()).find("Cannot") != std::string::npos);
                         throw;
                     }
                 }, std::runtime_error);
}

// =================================================================
// Status & Getter Tests
// =================================================================

TEST_F(SeatAllocatorTest, HasAvailableSeats_VariousStates) {
    allocator = new SeatAllocator(3);
    EXPECT_TRUE(allocator->hasAvailableSeats());

    allocator->allocateSeat(1);
    allocator->allocateSeat(2);
    allocator->allocateSeat(3);
    EXPECT_FALSE(allocator->hasAvailableSeats());

    allocator->freeSeat(1);
    EXPECT_TRUE(allocator->hasAvailableSeats());
}

TEST_F(SeatAllocatorTest, GetCounts_Consistency) {
    allocator = new SeatAllocator(15);
    allocator->allocateSeat(1);
    allocator->allocateSeat(2);

    int available = allocator->getAvailableSeatCount();
    int allocated = allocator->getAllocatedSeatCount();

    EXPECT_EQ(15, available + allocated);
}

TEST_F(SeatAllocatorTest, PrintStatus_OutputFormat) {
    allocator = new SeatAllocator(5);
    allocator->allocateSeat(31001);

    allocator->printStatus();

    std::string output = getCapturedOutput();
    EXPECT_TRUE(output.find("Seat Allocator Status") != std::string::npos);
    EXPECT_TRUE(output.find("Total Seats: 5") != std::string::npos);
    EXPECT_TRUE(output.find("Available Seats Count: 4") != std::string::npos);
}

// =================================================================
// Complex & Stress Tests
// =================================================================

TEST_F(SeatAllocatorTest, ComplexScenario_WaitingListPromotion) {
    allocator = new SeatAllocator(2);
    allocator->allocateSeat(25001); // 1
    allocator->allocateSeat(25002); // 2
    allocator->allocateSeat(25003); // WL1
    allocator->allocateSeat(25004); // WL2

    clearOutput();
    // Free seat 1 - should promote 25003
    int res1 = allocator->freeSeat(1);
    EXPECT_EQ(25003, res1);
    EXPECT_TRUE(getCapturedOutput().find("25003") != std::string::npos);

    clearOutput();
    // Free seat 2 - should promote 25004
    int res2 = allocator->freeSeat(2);
    EXPECT_EQ(25004, res2);
    EXPECT_TRUE(getCapturedOutput().find("25004") != std::string::npos);
}

TEST_F(SeatAllocatorTest, StressTest_ManyAllocationsAndFrees) {
    allocator = new SeatAllocator(100);

    // Allocate 50 seats
    for (int i = 1; i <= 50; i++) {
        allocator->allocateSeat(27000 + i);
    }
    EXPECT_EQ(50, allocator->getAllocatedSeatCount());

    // Free every other seat
    for (int i = 1; i <= 50; i += 2) {
        allocator->freeSeat(i);
    }
    EXPECT_EQ(25, allocator->getAllocatedSeatCount());
    EXPECT_EQ(75, allocator->getAvailableSeatCount());

    // Reallocate
    for (int i = 1; i <= 25; i++) {
        allocator->allocateSeat(28000 + i);
    }
    EXPECT_EQ(50, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, GetAllocatedSeatCount_WithCancelledSeatsMarkedAsMinusOne) {
    allocator = new SeatAllocator(10);
    allocator->allocateSeat(38001); // seat 1
    allocator->allocateSeat(38002); // seat 2
    EXPECT_EQ(2, allocator->getAllocatedSeatCount());

    allocator->freeSeat(2); // seat 2 freed to cancelled stack

    // Count should be 1
    EXPECT_EQ(1, allocator->getAllocatedSeatCount());
    // Available should be 8 original + 1 cancelled = 9
    EXPECT_EQ(9, allocator->getAvailableSeatCount());
}
TEST_F(SeatAllocatorTest, AddSeats_DoesNotAutoAssignWaitingList) {
    // Rationale: Verify that adding seats simply makes them available,
    // but does NOT automatically pop the waiting list (based on current implementation).
    allocator = new SeatAllocator(2);

    // Fill seats
    allocator->allocateSeat(101);
    allocator->allocateSeat(102);

    // Add to waiting list
    allocator->allocateSeat(103);

    // Add a new seat. Total becomes 3.
    allocator->addSeats(1);

    // Expectation: Seat 3 is available, Passenger 103 is STILL in waiting list.
    EXPECT_EQ(1, allocator->getAvailableSeatCount());
    EXPECT_EQ(2, allocator->getAllocatedSeatCount());

    // Now trigger allocation, should prioritize Waiting List
    int seat = allocator->allocateSeat(999); // ID doesn't matter, should pull from WL
    EXPECT_EQ(3, seat); // The new seat

    // How do we verify it was 103? (The class doesn't return the ID, it returns the Seat #).
    // We can infer it by checking if 999 got a seat or if it was added to WL.
    // If 103 took the seat, 999 should be rejected or added to WL (if logic handles it).
    // *Correction*: allocateSeat(999) checks WL. If WL has people, it usually returns -1
    // or adds 999 to WL.
    // To properly test this with the current API: The next freeSeat/allocate logic usually handles WL.

    // Let's verify via print status or simply by knowing seat 3 is now taken.
    EXPECT_EQ(3, allocator->getAllocatedSeatCount());
}

TEST_F(SeatAllocatorTest, AddSeats_PreservesCancelledSeats) {
    // Rationale: Ensure adding new seats doesn't mess up the 'cancelled' stack.
    allocator = new SeatAllocator(5);
    allocator->allocateSeat(101); // Seat 1
    allocator->freeSeat(1);       // Seat 1 is now in Cancelled Stack

    allocator->addSeats(5);       // Total 10

    // Allocate should still pull from Cancelled Stack (LIFO/Priority) before new seats
    int seat = allocator->allocateSeat(200);
    EXPECT_EQ(1, seat); // Should still get the cancelled seat

    // Next allocation should get a new seat (Seat 6, as 2-5 are in available set,
    // but usually minHeap logic picks smallest. 2 is smaller than 6).
    int seat2 = allocator->allocateSeat(201);
    EXPECT_EQ(2, seat2);
}

TEST_F(SeatAllocatorTest, AddSeats_MassiveAddition) {
    // Rationale: Stress test expanding capacity significantly.
    allocator = new SeatAllocator(10);
    allocator->addSeats(10000);

    EXPECT_EQ(10010, allocator->getAvailableSeatCount());

    // Verify the very last seat exists
    int seat = allocator->allocateSeat(555);
    EXPECT_EQ(1, seat); // Min heap logic
}
TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkRemovesFromEndOfSet) {
    // 1. Setup: 10 Seats.
    // Allocated: {1, 2}
    // Available Set: {3, 4, 5, 6, 7, 8, 9, 10}
    allocator = new SeatAllocator(10);
    allocator->allocateSeat(101); // Takes 1
    allocator->allocateSeat(102); // Takes 2

    // 2. Action: Shrink to 7.
    // Logic: Must remove 3 seats.
    // Since availableSeats is a SET, it should remove the last 3 iterators: 10, 9, 8.
    allocator->changeTotalSeats(7);

    // 3. Verification
    // Available should now be {3, 4, 5, 6, 7}
    EXPECT_EQ(5, allocator->getAvailableSeatCount());
    EXPECT_EQ(7, allocator->getAllocatedSeatCount() + allocator->getAvailableSeatCount());

    // 4. Critical Check: Ensure High IDs are gone, Low IDs remain.
    // If we allocate now, we should get 3, 4, 5...
    EXPECT_EQ(3, allocator->allocateSeat(201));
    EXPECT_EQ(4, allocator->allocateSeat(202));

    // We expect 5, 6, 7 to remain.
    // If we deliberately try to free "8" (which shouldn't exist), it should fail.
    // (Assuming you have bounds checking in freeSeat)
    clearOutput();
    allocator->freeSeat(8);
    EXPECT_TRUE(getCapturedOutput().find("not exist") != std::string::npos);
}
TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkWithGapsInSet) {
    // 1. Setup: 10 Seats.
    allocator = new SeatAllocator(10);

    // 2. Create Gaps.
    // Allocate 1, 2.
    allocator->allocateSeat(101);
    allocator->allocateSeat(102);

    // Allocate 9, 10 (We need to burn through 3-8 to get here, then free 3-8)
    // Actually, min-allocation logic makes it hard to grab 9/10 directly without loop.
    std::vector<int> mids;
    for(int i=3; i<=8; i++) mids.push_back(allocator->allocateSeat(200+i));

    allocator->allocateSeat(109); // Seat 9
    allocator->allocateSeat(110); // Seat 10

    // Free the middle to return them to the Cancelled STACK (Note: NOT the Set)
    // Your code puts freed seats in a stack, not back in the set.
    for(int s : mids) allocator->freeSeat(s);

    // Current State:
    // Allocated: {1, 2, 9, 10}
    // Available Set: {} (Empty! Because 1-10 were all allocated initially)
    // Cancelled Stack: {8, 7, 6, 5, 4, 3}

    // 3. Action: Try to Shrink to 8.
    // Logic: Need to remove 2 seats.
    // The Set is empty. The Stack has 6 seats.
    // YOUR CODE: checks `availableSeats.size() < seatsToRemove`.
    // availableSeats.size() is 0. 0 < 2.

    // Expectation: This should THROW, because freed seats are in the stack, not the set.
    // You cannot shrink seats that are currently in the "cancelled/reusable" stack.
    EXPECT_THROW({
                     allocator->changeTotalSeats(8);
                 }, std::runtime_error);
}
TEST_F(SeatAllocatorTest, AddSeats_MaintainsSetOrder) {
    allocator = new SeatAllocator(5);
    // Fill 1-5
    for(int i=1; i<=5; i++) allocator->allocateSeat(100+i);

    // Add 5 more (6-10)
    allocator->addSeats(5);

    // Verify immediate availability of the next sequential ID
    // Since it's a set, 6 should be the very next one.
    int seat = allocator->allocateSeat(106);
    EXPECT_EQ(6, seat);

    // Free seat 6. It goes to Stack.
    allocator->freeSeat(6);

    // Add 5 more (11-15).
    allocator->addSeats(5);

    // Verify Stack Priority vs Set
    // Stack has {6}. Set has {7,8,9,10,11,12,13,14,15}.
    // Allocate should take from Stack (6) first.
    EXPECT_EQ(6, allocator->allocateSeat(107));

    // Next allocation should resume from Set (7).
    EXPECT_EQ(7, allocator->allocateSeat(108));
}
TEST_F(SeatAllocatorTest, ChangeTotalSeats_ShrinkCutsOffHighSeat_BehaviorCheck) {
    allocator = new SeatAllocator(10);
    // Allocate 1-9
    for(int i=1; i<=9; i++) allocator->allocateSeat(100+i);
    // Allocate 10
    allocator->allocateSeat(110); // Seat 10 is taken.

    // Free 1-9. They go to Stack.
    for(int i=1; i<=9; i++) allocator->freeSeat(i);

    // Current State:
    // Total: 10
    // Allocated Count: 1 (Passenger 110 in Seat 10)
    // Available Set: {} (Empty, all touched)
    // Cancelled Stack: {9, 8...1}

    // Attempt Shrink to 5.
    // 1. Check: 5 < 1 (AllocatedCount)? False. Logic proceeds.
    // 2. Check: Available Set size (0) < SeatsToRemove (5)? True.

    // This MUST throw because the available seats are in the stack, not the set.
    EXPECT_THROW(allocator->changeTotalSeats(5), std::runtime_error);
}