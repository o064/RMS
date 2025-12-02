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

TEST_F(IntegrationTest, CompleteBookingFlow) {
    Train train(1, "Express", 5);
    Passenger passenger(101, "John Doe");

    int seatNumber = train.getSeatAllocator()->allocateSeat(passenger.getId());
    EXPECT_GT(seatNumber, 0);

    Ticket ticket(1, seatNumber, train.getTrainId(), passenger);
    EXPECT_EQ(ticket.getSeat(), seatNumber);
    EXPECT_EQ(ticket.getTrainId(), train.getTrainId());
    EXPECT_EQ(ticket.getPassenger().getId(), passenger.getId());
    EXPECT_EQ(ticket.getStatus(), booked);
}

TEST_F(IntegrationTest, MultiplePassengersBooking) {
    Train train(1, "Express", 10);
    std::vector<Passenger> passengers;
    std::vector<Ticket> tickets;

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

TEST_F(IntegrationTest, BookingCancellationFlow) {
    Train train(1, "Express", 5);
    Passenger passenger(101, "John Doe");

    int seatNumber = train.getSeatAllocator()->allocateSeat(passenger.getId());
    Ticket ticket(1, seatNumber, train.getTrainId(), passenger);

    ticket.setStatus(cancelled);
    int result = train.getSeatAllocator()->freeSeat(seatNumber);

    EXPECT_EQ(ticket.getStatus(), cancelled);
    EXPECT_EQ(result, 0);
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

    ticket1.setStatus(cancelled);
    train.getSeatAllocator()->freeSeat(seat1);

    Passenger p3(103, "Bob");
    int seat3 = train.getSeatAllocator()->allocateSeat(p3.getId());
    EXPECT_EQ(seat3, seat1);
}

TEST_F(IntegrationTest, WaitingListBasicFlow) {
    Train train(1, "Express", 2);

    Passenger p1(101, "John");
    Passenger p2(102, "Jane");
    Passenger p3(103, "Bob");

    train.getSeatAllocator()->allocateSeat(p1.getId());
    train.getSeatAllocator()->allocateSeat(p2.getId());
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
    train.getSeatAllocator()->allocateSeat(p3.getId());

    int newPassenger = train.getSeatAllocator()->freeSeat(seat1);
    EXPECT_EQ(newPassenger, p3.getId());
    EXPECT_TRUE(train.getSeatAllocator()->getWaitingList().empty());
}

TEST_F(IntegrationTest, AddSeatsWithActiveBookings) {
    Train train(1, "Express", 5);

    train.getSeatAllocator()->allocateSeat(101);
    train.getSeatAllocator()->allocateSeat(102);
    train.getSeatAllocator()->allocateSeat(103);

    train.addSeats(5);

    EXPECT_EQ(train.getTotalSeats(), 10);
    EXPECT_EQ(train.getSeatAllocator()->getAllocatedSeatCount(), 3);
    EXPECT_EQ(train.getSeatAllocator()->getAvailableSeatCount(), 7);
}

TEST_F(IntegrationTest, FullOperationalDay) {
    Train train(1, "Morning Express", 5);
    std::vector<Ticket> activeTickets;

    for (int i = 1; i <= 5; i++) {
        Passenger p(100 + i, "Passenger " + std::to_string(i));
        int seat = train.getSeatAllocator()->allocateSeat(p.getId());
        activeTickets.push_back(Ticket(i, seat, train.getTrainId(), p));
    }

    EXPECT_FALSE(train.hasAvailableSeats());

    activeTickets[0].setStatus(cancelled);
    activeTickets[2].setStatus(cancelled);
    train.getSeatAllocator()->freeSeat(activeTickets[0].getSeat());
    train.getSeatAllocator()->freeSeat(activeTickets[2].getSeat());

    EXPECT_TRUE(train.hasAvailableSeats());

    Passenger p6(106, "Late Passenger 1");
    Passenger p7(107, "Late Passenger 2");
    int seat6 = train.getSeatAllocator()->allocateSeat(p6.getId());
    int seat7 = train.getSeatAllocator()->allocateSeat(p7.getId());

    EXPECT_GT(seat6, 0);
    EXPECT_GT(seat7, 0);
    EXPECT_FALSE(train.hasAvailableSeats());
}

TEST_F(IntegrationTest, SingleSeatTrainOperations) {
    Train train(1, "Mini Train", 1);
    Passenger p1(101, "John");

    int seat = train.getSeatAllocator()->allocateSeat(p1.getId());
    EXPECT_EQ(seat, 1);
    EXPECT_FALSE(train.hasAvailableSeats());

    Passenger p2(102, "Jane");
    int seat2 = train.getSeatAllocator()->allocateSeat(p2.getId());
    EXPECT_EQ(seat2, -1);

    train.getSeatAllocator()->freeSeat(1);
    EXPECT_TRUE(train.getSeatAllocator()->getWaitingList().empty());
}