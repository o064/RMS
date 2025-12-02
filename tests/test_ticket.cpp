#include <gtest/gtest.h>
#include "models/Ticket.h"
#include "models/Passenger.h"

class TicketTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TicketTest, ValidConstruction) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    EXPECT_EQ(ticket.getId(), 1);
    EXPECT_EQ(ticket.getSeat(), 5);
    EXPECT_EQ(ticket.getTrainId(), 10);
    EXPECT_EQ(ticket.getStatus(), booked);
    EXPECT_EQ(ticket.getPassenger().getId(), 101);
    EXPECT_EQ(ticket.getPassenger().getName(), "John Doe");
}

TEST_F(TicketTest, ConstructionWithDifferentPassenger) {
    Passenger p(202, "Jane Smith");
    Ticket ticket(2, 10, 5, p);
    EXPECT_EQ(ticket.getId(), 2);
    EXPECT_EQ(ticket.getSeat(), 10);
    EXPECT_EQ(ticket.getTrainId(), 5);
    EXPECT_EQ(ticket.getPassenger().getId(), 202);
}

TEST_F(TicketTest, DefaultStatusIsBooked) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    EXPECT_EQ(ticket.getStatus(), booked);
}

TEST_F(TicketTest, InvalidTicketIdNegative) {
    Passenger p(101, "John Doe");
    EXPECT_THROW(Ticket(-1, 5, 10, p), std::invalid_argument);
}

TEST_F(TicketTest, InvalidTicketIdZero) {
    Passenger p(101, "John Doe");
    Ticket t(0, 5, 10, p);
    EXPECT_THROW(t.setId(0), std::invalid_argument);
}

TEST_F(TicketTest, InvalidTrainIdNegative) {
    Passenger p(101, "John Doe");
    EXPECT_THROW(Ticket(1, 5, -1, p), std::invalid_argument);
}

TEST_F(TicketTest, InvalidTrainIdZero) {
    Passenger p(101, "John Doe");
    EXPECT_THROW(Ticket(1, 5, 0, p), std::invalid_argument);
}

TEST_F(TicketTest, InvalidSeatZero) {
    Passenger p(101, "John Doe");
    EXPECT_THROW(Ticket(1, 0, 10, p), std::invalid_argument);
}

TEST_F(TicketTest, InvalidSeatNegative) {
    Passenger p(101, "John Doe");
    EXPECT_THROW(Ticket(1, -5, 10, p), std::invalid_argument);
}

TEST_F(TicketTest, SetStatusToBooked) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    ticket.setStatus(booked);
    EXPECT_EQ(ticket.getStatus(), booked);
}

TEST_F(TicketTest, SetStatusToCancelled) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    ticket.setStatus(cancelled);
    EXPECT_EQ(ticket.getStatus(), cancelled);
}

TEST_F(TicketTest, ToggleStatusMultipleTimes) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    ticket.setStatus(cancelled);
    EXPECT_EQ(ticket.getStatus(), cancelled);
    ticket.setStatus(booked);
    EXPECT_EQ(ticket.getStatus(), booked);
    ticket.setStatus(cancelled);
    EXPECT_EQ(ticket.getStatus(), cancelled);
}

TEST_F(TicketTest, SetIdChangesTicketId) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    ticket.setId(100);
    EXPECT_EQ(ticket.getId(), 100);
}

TEST_F(TicketTest, SetIdDoesNotAffectOtherFields) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    ticket.setId(200);
    EXPECT_EQ(ticket.getId(), 200);
    EXPECT_EQ(ticket.getSeat(), 5);
    EXPECT_EQ(ticket.getTrainId(), 10);
    EXPECT_EQ(ticket.getPassenger().getId(), 101);
}

TEST_F(TicketTest, BookedToCancelledWorkflow) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    EXPECT_EQ(ticket.getStatus(), booked);
    ticket.setStatus(cancelled);
    EXPECT_EQ(ticket.getStatus(), cancelled);
}

TEST_F(TicketTest, PassengerDataIndependent) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 5, 10, p);
    p.setName("Modified Name");
    EXPECT_EQ(ticket.getPassenger().getName(), "John Doe");
}

TEST_F(TicketTest, MultipleTicketsSamePassenger) {
    Passenger p(101, "John Doe");
    Ticket ticket1(1, 5, 10, p);
    Ticket ticket2(2, 6, 10, p);
    EXPECT_EQ(ticket1.getPassenger().getId(), 101);
    EXPECT_EQ(ticket2.getPassenger().getId(), 101);
    EXPECT_NE(ticket1.getId(), ticket2.getId());
    EXPECT_NE(ticket1.getSeat(), ticket2.getSeat());
}

TEST_F(TicketTest, LargeTicketId) {
    Passenger p(101, "John Doe");
    Ticket ticket(999999, 5, 10, p);
    EXPECT_EQ(ticket.getId(), 999999);
}

TEST_F(TicketTest, LargeSeatNumber) {
    Passenger p(101, "John Doe");
    Ticket ticket(1, 10000, 10, p);
    EXPECT_EQ(ticket.getSeat(), 10000);
}

TEST_F(TicketTest, AllMaximumValues) {
    Passenger p(999999, "John Doe");
    Ticket ticket(999999, 999999, 999999, p);
    EXPECT_EQ(ticket.getId(), 999999);
    EXPECT_EQ(ticket.getSeat(), 999999);
    EXPECT_EQ(ticket.getTrainId(), 999999);
    EXPECT_EQ(ticket.getPassenger().getId(), 999999);
}