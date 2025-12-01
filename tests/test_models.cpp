//
// Created by Omar on 12/1/2025.
//

// ============================================================================
// IntegrationTest.cpp - Integration Tests for Railway Management System
// ============================================================================

#include <gtest/gtest.h>
#include "models/Passenger.h"
#include "models/Train.h"
#include "models/SeatAllocator.h"
#include "models/Ticket.h"
#include <vector>

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// BASIC BOOKING FLOW TESTS
// ============================================================================

TEST_F(IntegrationTest, CompleteBookingFlow) {
// Create train
Train train(1, "Express", 5);

// Create passenger
Passenger passenger(101, "John Doe");

// Allocate seat
int seatNumber = train.getSeatAllocator()->allocateSeat(passenger.getId());
EXPECT_GT(seatNumber, 0);

// Create ticket
Ticket ticket(1, seatNumber, train.getTrainId(), passenger);

// Verify ticket details
EXPECT_EQ(ticket.getSeat(), seatNumber);
EXPECT_EQ(ticket.getTrainId(), train.getTrainId());
EXPECT_EQ(ticket.getPassenger().getId(), passenger.getId());
EXPECT_EQ(ticket.getStatus(), booked);
}

TEST_F(IntegrationTest, MultiplePassengersBooking) {
Train train(1, "Express", 10);
std::vector<Passenger> passengers;
std::vector<Ticket> tickets;

// Create and book for 5 passengers
for (int i = 1; i <= 5; i++) {
Passenger p(100 + i, "Passenger " + std::to_string(i));
passengers.push_back(p);

int seat = train.getSeatAllocator()->allocateSeat(p.getId());
EXPECT_EQ(seat, i);

Ticket ticket(i, seat, train.getTrainId(), p);
tickets.push_back(ticket);
}

EXPECT_EQ(tickets.size(), 5);
EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 5);
EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(IntegrationTest, BookingUntilFull) {
Train train(1, "Express", 3);

Passenger p1(101, "John");
Passenger p2(102, "Jane");
Passenger p3(103, "Bob");

int seat1 = train.getSeatAllocator()->allocateSeat(p1.getId());
int seat2 = train.getSeatAllocator()->allocateSeat(p2.getId());
int seat3 = train.getSeatAllocator()->allocateSeat(p3.getId());

EXPECT_EQ(seat1, 1);
EXPECT_EQ(seat2, 2);
EXPECT_EQ(seat3, 3);
EXPECT_FALSE(train.hasAvailableSeats());
}

// ============================================================================
// CANCELLATION FLOW TESTS
// ============================================================================

TEST_F(IntegrationTest, BookingCancellationFlow) {
Train train(1, "Express", 5);
Passenger passenger(101, "John Doe");

int seatNumber = train.getSeatAllocator()->allocateSeat(passenger.getId());
Ticket ticket(1, seatNumber, train.getTrainId(), passenger);

// Cancel ticket
ticket.setStatus(cancelled);
int result = train.getSeatAllocator()->freeSeat(seatNumber);

EXPECT_EQ(ticket.getStatus(), cancelled);
EXPECT_EQ(result, 0); // No waiting passenger
EXPECT_TRUE(train.hasAvailableSeats());
}

TEST_F(IntegrationTest, CancellationWithReallocation) {
Train train(1, "Express", 2);

Passenger p1(101, "John");
Passenger p2(102, "Jane");

int seat1 = train.getSeatAllocator()->allocateSeat(p1.getId());
int seat2 = train.getSeatAllocator()->allocateSeat(p2.getId());

Ticket ticket1(1, seat1, train.getTrainId(), p1);
Ticket ticket2(2, seat2, train.getTrainId(), p2);

// Cancel ticket1 and reallocate
ticket1.setStatus(cancelled);
train.getSeatAllocator()->freeSeat(seat1);

Passenger p3(103, "Bob");
int seat3 = train.getSeatAllocator()->allocateSeat(p3.getId());

EXPECT_EQ(seat3, seat1); // Should get cancelled seat
}

TEST_F(IntegrationTest, MultipleCancellationsSequence) {
Train train(1, "Express", 5);
std::vector<Ticket> tickets;

// Book all seats
for (int i = 1; i <= 5; i++) {
Passenger p(100 + i, "Passenger " + std::to_string(i));
int seat = train.getSeatAllocator()->allocateSeat(p.getId());
tickets.push_back(Ticket(i, seat, train.getTrainId(), p));
}

// Cancel tickets 1, 2, 3
for (int i = 0; i < 3; i++) {
tickets[i].setStatus(cancelled);
train.getSeatAllocator()->freeSeat(tickets[i].getSeat());
}

EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 3);
EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 2);
}

// ============================================================================
// WAITING LIST FLOW TESTS
// ============================================================================

TEST_F(IntegrationTest, WaitingListBasicFlow) {
Train train(1, "Express", 2);

Passenger p1(101, "John");
Passenger p2(102, "Jane");
Passenger p3(103, "Bob");

// Fill train
train.getSeatAllocator()->allocateSeat(p1.getId());
train.getSeatAllocator()->allocateSeat(p2.getId());

// Try to book when full
int seat3 = train.getSeatAllocator()->allocateSeat(p3.getId());
EXPECT_EQ(seat3, -1);
EXPECT_FALSE(train.getSeatAllocator()->getWaitingList().empty());
}

TEST_F(IntegrationTest, WaitingListAutoAllocation) {
Train train(1, "Express", 2);

Passenger p1(101, "John");
Passenger p2(102, "Jane");
Passenger p3(103, "Bob");

int seat1 = train.getSeatAllocator()->allocateSeat(p1.getId());
int seat2 = train.getSeatAllocator()->allocateSeat(p2.getId());

// Add to waiting list
train.getSeatAllocator()->allocateSeat(p3.getId());

// Cancel first booking - should auto-assign to waiting passenger
int newPassenger = train.getSeatAllocator()->freeSeat(seat1);
EXPECT_EQ(newPassenger, p3.getId());

// Verify waiting list is empty
EXPECT_TRUE(train.getSeatAllocator()->getWaitingList().empty());
}

TEST_F(IntegrationTest, MultipleWaitingPassengers) {
Train train(1, "Express", 2);

// Fill train
train.getSeatAllocator()->allocateSeat(101);
train.getSeatAllocator()->allocateSeat(102);

// Add multiple to waiting list
train.getSeatAllocator()->allocateSeat(103);
train.getSeatAllocator()->allocateSeat(104);
train.getSeatAllocator()->allocateSeat(105);

std::queue<int> waiting = train.getSeatAllocator()->getWaitingList();
EXPECT_EQ(waiting.size(), 3);

// Free one seat - first waiting gets it
int assigned = train.getSeatAllocator()->freeSeat(1);
EXPECT_EQ(assigned, 103);

waiting = train.getSeatAllocator()->getWaitingList();
EXPECT_EQ(waiting.size(), 2);
}

// ============================================================================
// TRAIN CAPACITY MODIFICATION TESTS
// ============================================================================

TEST_F(IntegrationTest, AddSeatsWithActiveBookings) {
Train train(1, "Express", 5);

// Book 3 seats
train.getSeatAllocator()->allocateSeat(101);
train.getSeatAllocator()->allocateSeat(102);
train.getSeatAllocator()->allocateSeat(103);

// Add more seats
train.addSeats(5);

EXPECT_EQ(train.getTotalSeats(), 10);
EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 3);
EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 7);
}

TEST_F(IntegrationTest, ExpandTrainToAccommodateWaitingList) {
Train train(1, "Express", 2);

// Fill train and add to waiting list
train.getSeatAllocator()->allocateSeat(101);
train.getSeatAllocator()->allocateSeat(102);
train.getSeatAllocator()->allocateSeat(103); // Waiting
train.getSeatAllocator()->allocateSeat(104); // Waiting

// Expand train
train.addSeats(3);

// Now allocate waiting passengers
EXPECT_TRUE(train.hasAvailableSeats());

// Waiting passengers can now be allocated
int seat3 = train.getSeatAllocator()->allocateSeat(105);
EXPECT_GT(seat3, 0);
}

TEST_F(IntegrationTest, ShrinkTrainWithUnusedCapacity) {
Train train(1, "Express", 10);

// Book only 3 seats
train.getSeatAllocator()->allocateSeat(101);
train.getSeatAllocator()->allocateSeat(102);
train.getSeatAllocator()->allocateSeat(103);

// Shrink to 5 seats (still have space)
train.setSeats(5);

EXPECT_EQ(train.getTotalSeats(), 5);
EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 3);
}

// ============================================================================
// COMPLEX SCENARIO TESTS
// ============================================================================

TEST_F(IntegrationTest, FullOperationalDay) {
Train train(1, "Morning Express", 5);
std::vector<Ticket> activeTickets;

// Morning rush - fill train
for (int i = 1; i <= 5; i++) {
Passenger p(100 + i, "Passenger " + std::to_string(i));
int seat = train.getSeatAllocator()->allocateSeat(p.getId());
activeTickets.push_back(Ticket(i, seat, train.getTrainId(), p));
}

EXPECT_FALSE(train.hasAvailableSeats());

// Two passengers cancel
activeTickets[0].setStatus(cancelled);
activeTickets[2].setStatus(cancelled);
train.getSeatAllocator()->freeSeat(activeTickets[0].getSeat());
train.getSeatAllocator()->freeSeat(activeTickets[2].getSeat());

EXPECT_TRUE(train.hasAvailableSeats());

// New bookings
Passenger p6(106, "Late Passenger 1");
Passenger p7(107, "Late Passenger 2");
int seat6 = train.getSeatAllocator()->allocateSeat(p6.getId());
int seat7 = train.getSeatAllocator()->allocateSeat(p7.getId());

EXPECT_GT(seat6, 0);
EXPECT_GT(seat7, 0);
EXPECT_FALSE(train.hasAvailableSeats());
}

TEST_F(IntegrationTest, TrainUpgradeScenario) {
Train train(1, "Local Train", 10);

// Book 7 seats
for (int i = 1; i <= 7; i++) {
train.getSeatAllocator()->allocateSeat(100 + i);
}

// Train upgraded - add more seats
train.setTrainName("Express Train");
train.addSeats(10);

EXPECT_EQ(train.getTrainName(), "Express Train");
EXPECT_EQ(train.getTotalSeats(), 20);
EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 7);
EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 13);
}

TEST_F(IntegrationTest, PeakHourWithWaitingList) {
Train train(1, "Express", 3);

// Fill train
Passenger p1(101, "John");
Passenger p2(102, "Jane");
Passenger p3(103, "Bob");

train.getSeatAllocator()->allocateSeat(p1.getId());
train.getSeatAllocator()->allocateSeat(p2.getId());
train.getSeatAllocator()->allocateSeat(p3.getId());

// Add 3 to waiting list
Passenger p4(104, "Alice");
Passenger p5(105, "Charlie");
Passenger p6(106, "Diana");

train.getSeatAllocator()->allocateSeat(p4.getId());
train.getSeatAllocator()->allocateSeat(p5.getId());
train.getSeatAllocator()->allocateSeat(p6.getId());

std::queue<int> waiting = train.getSeatAllocator()->getWaitingList();
EXPECT_EQ(waiting.size(), 3);

// One cancellation
int newPassenger = train.getSeatAllocator()->freeSeat(1);
EXPECT_EQ(newPassenger, p4.getId());

waiting = train.getSeatAllocator()->getWaitingList();
EXPECT_EQ(waiting.size(), 2);
}

// ============================================================================
// EDGE CASE INTEGRATION TESTS
// ============================================================================

TEST_F(IntegrationTest, SingleSeatTrainOperations) {
Train train(1, "Mini Train", 1);
Passenger p1(101, "John");

int seat = train.getSeatAllocator()->allocateSeat(p1.getId());
EXPECT_EQ(seat, 1);
EXPECT_FALSE(train.hasAvailableSeats());

Passenger p2(102, "Jane");
int seat2 = train.getSeatAllocator()->allocateSeat(p2.getId());
EXPECT_EQ(seat2, -1); // Goes to waiting

train.getSeatAllocator()->freeSeat(1);
// p2 should now have seat 1
EXPECT_TRUE(train.getSeatAllocator()->getWaitingList().empty());
}

TEST_F(IntegrationTest, PassengerWithMultipleBookingAttempts) {
Train train(1, "Express", 5);
Passenger passenger(101, "John Doe");

int seat1 = train.getSeatAllocator()->allocateSeat(passenger.getId());
EXPECT_GT(seat1, 0);

// Try to book again with same ID
int seat2 = train.getSeatAllocator()->allocateSeat(passenger.getId());
EXPECT_EQ(seat2, -1); // Should fail
}

TEST_F(IntegrationTest, CopyTrainWithBookings) {
Train train(1, "Express", 5);

train.getSeatAllocator()->allocateSeat(101);
train.getSeatAllocator()->allocateSeat(102);
train.getSeatAllocator()->allocateSeat(103);

Train trainCopy(train);

EXPECT_EQ(trainCopy.getTotalSeats(), train.getTotalSeats());
EXPECT_EQ(trainCopy.getSeatAllocator()->getAllocatedSeatCount(),
train.getSeatAllocator()->getAllocatedSeatCount());

// Verify independence
trainCopy.getSeatAllocator()->allocateSeat(104);
EXPECT_NE(trainCopy.getSeatAllocator()->getAllocatedSeatCount(),
train.getSeatAllocator()->getAllocatedSeatCount());
}

// ============================================================================
// DATA CONSISTENCY TESTS
// ============================================================================

TEST_F(IntegrationTest, TicketDataMatchesAllocation) {
Train train(1, "Express", 5);
Passenger passenger(101, "John Doe");

int allocatedSeat = train.getSeatAllocator()->allocateSeat(passenger.getId());
Ticket ticket(1, allocatedSeat, train.getTrainId(), passenger);

EXPECT_EQ(ticket.getSeat(), allocatedSeat);
EXPECT_EQ(ticket.getTrainId(), train.getTrainId());
EXPECT_EQ(ticket.getPassenger().getId(), passenger.getId());
EXPECT_EQ(ticket.getPassenger().getName(), passenger.getName());
}

TEST_F(IntegrationTest, ConsistencyAfterMultipleOperations) {
Train train(1, "Express", 10);

// Multiple allocations and cancellations
std::vector<int> seats;
for (int i = 1; i <= 5; i++) {
int seat = train.getSeatAllocator()->allocateSeat(100 + i);
seats.push_back(seat);
}

train.getSeatAllocator()->freeSeat(seats[1]);
train.getSeatAllocator()->freeSeat(seats[3]);

train.getSeatAllocator()->allocateSeat(201);
train.getSeatAllocator()->allocateSeat(202);

// Check consistency
int total = train.getTotalSeats();
int allocated = train.getSeatAllocator()->getAllocatedSeatCount();
int available = train.getSeatAllocator()->getAvailableSeatCount();

EXPECT_EQ(total, allocated + available);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}