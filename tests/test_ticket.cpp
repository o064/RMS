//
// Created by Omar on 11/24/2025.
//
#include "models/Ticket.h"
#include "models/Passenger.h"
#include <gtest/gtest.h>

// Helper function to create a sample Passenger
Passenger createTestPassenger() {
    return Passenger(1, "Omar");
}

TEST(TicketTest, ConstructorAndGetters) {
Passenger p = createTestPassenger();
Ticket t(1, 10, 101, p);

EXPECT_EQ(t.getId(), 1);
EXPECT_EQ(t.getSeat(), 10);
EXPECT_EQ(t.getTrainId(), 101);

// Check passenger data
EXPECT_EQ(t.getPassenger().getId(), 1);
EXPECT_EQ(t.getPassenger().getName(), "Omar");

// Default status should be booked
EXPECT_EQ(t.getStatus(), booked);
}

TEST(TicketTest, SetStatus) {
Passenger p = createTestPassenger();
Ticket t(2, 20, 102, p);

t.setStatus(cancelled);
EXPECT_EQ(t.getStatus(), cancelled);

t.setStatus(booked);
EXPECT_EQ(t.getStatus(), booked);
}

TEST(TicketTest, SetId) {
Passenger p = createTestPassenger();
Ticket t(3, 15, 103, p);

t.setId(100);
EXPECT_EQ(t.getId(), 100);
}
