//
// Created by Omar on 11/24/2025.
//

#include <gtest/gtest.h>
#include "Services/TicketService.h"
#include "Services/TrainService.h"
#include "Services/PassengerService.h"
#include "Repo/InMemoryTicketRepository.h"
#include "Repo/InMemoryTrainRepository.h"
#include "Repo/InMemoryPassengerRepository.h"

class TicketServiceTest : public ::testing::Test {
protected:
    // Repositories
    InMemoryTicketRepository* ticketRepo;
    InMemoryTrainRepository* trainRepo;
    InMemoryPassengerRepository* passengerRepo;

    // Services
    TrainService* trainService;
    PassengerService* passengerService;
    TicketService* ticketService;

    void SetUp() override {
        ticketRepo = new InMemoryTicketRepository();
        trainRepo = new InMemoryTrainRepository();
        passengerRepo = new InMemoryPassengerRepository();

        trainService = new TrainService(trainRepo);
        passengerService = new PassengerService(passengerRepo);
        ticketService = new TicketService(ticketRepo, trainService, passengerService);
    }

    void TearDown() override {
        delete ticketService;
        delete passengerService;
        delete trainService;
        delete ticketRepo;
        delete trainRepo;
        delete passengerRepo;
    }
};

// ================= EXISTING TESTS =================

TEST_F(TicketServiceTest, BookTicket_HappyPath) {
    Train t = trainService->createTrain("Express", 10);
    Passenger p = passengerService->createPassenger("Omar");

    Ticket ticket = ticketService->bookTicket(t.getTrainId(), p.getId());

    EXPECT_GT(ticket.getId(), 0);
    EXPECT_EQ(ticket.getTrainId(), t.getTrainId());
    EXPECT_EQ(ticket.getPassenger().getId(), p.getId());
    EXPECT_EQ(ticket.getStatus(), booked);

    auto savedTicket = ticketService->getTicket(ticket.getId());
    ASSERT_TRUE(savedTicket.has_value());
}

TEST_F(TicketServiceTest, BookTicket_ThrowsIfTrainNotFound) {
    Passenger p = passengerService->createPassenger("Omar");
    EXPECT_THROW(ticketService->bookTicket(999, p.getId()), std::runtime_error);
}

TEST_F(TicketServiceTest, BookTicket_ThrowsIfPassengerNotFound) {
    Train t = trainService->createTrain("Express", 10);
    EXPECT_THROW(ticketService->bookTicket(t.getTrainId(), 999), std::runtime_error);
}

TEST_F(TicketServiceTest, BookTicket_ThrowsIfAlreadyBooked) {
    Train t = trainService->createTrain("Express", 10);
    Passenger p = passengerService->createPassenger("Omar");

    ticketService->bookTicket(t.getTrainId(), p.getId());

    EXPECT_THROW(ticketService->bookTicket(t.getTrainId(), p.getId()), std::runtime_error);
}

TEST_F(TicketServiceTest, BookTicket_AssignsSeatsCorrectly) {
    Train t = trainService->createTrain("Express", 2);
    Passenger p1 = passengerService->createPassenger("P1");
    Passenger p2 = passengerService->createPassenger("P2");

    Ticket ticket1 = ticketService->bookTicket(t.getTrainId(), p1.getId());
    Ticket ticket2 = ticketService->bookTicket(t.getTrainId(), p2.getId());

    EXPECT_NE(ticket1.getSeat(), ticket2.getSeat());
    EXPECT_GT(ticket1.getSeat(), 0);
    EXPECT_GT(ticket2.getSeat(), 0);
}

TEST_F(TicketServiceTest, CancelTicket_Success) {
    Train t = trainService->createTrain("Express", 10);
    Passenger p = passengerService->createPassenger("Omar");
    Ticket ticket = ticketService->bookTicket(t.getTrainId(), p.getId());

    ticketService->cancelTicket(ticket.getId());

    auto updatedTicket = ticketService->getTicket(ticket.getId());
    ASSERT_TRUE(updatedTicket.has_value());
    EXPECT_EQ(updatedTicket->getStatus(), cancelled);
}

TEST_F(TicketServiceTest, CancelTicket_ThrowsIfNotFound) {
    EXPECT_THROW(ticketService->cancelTicket(9999), std::runtime_error);
}

TEST_F(TicketServiceTest, CancelTicket_FreesSeat) {
    Train t = trainService->createTrain("Express", 1);
    Passenger p1 = passengerService->createPassenger("P1");
    Passenger p2 = passengerService->createPassenger("P2");

    Ticket t1 = ticketService->bookTicket(t.getTrainId(), p1.getId());
    ticketService->cancelTicket(t1.getId());

    EXPECT_NO_THROW(ticketService->bookTicket(t.getTrainId(), p2.getId()));
}

TEST_F(TicketServiceTest, CancelTicket_ThrowsIfAlreadyCancelled) {
    Train t = trainService->createTrain("Express", 10);
    Passenger p = passengerService->createPassenger("Omar");
    Ticket ticket = ticketService->bookTicket(t.getTrainId(), p.getId());

    ticketService->cancelTicket(ticket.getId());

    EXPECT_THROW(ticketService->cancelTicket(ticket.getId()), std::runtime_error);
}

// ================= NEW EXPANDED TESTS =================

TEST_F(TicketServiceTest, BookTicket_ThrowsWhenTrainIsFull) {
    // 1. Create a train with exactly 1 seat
    Train t = trainService->createTrain("TinyTrain", 1);
    Passenger p1 = passengerService->createPassenger("Alice");
    Passenger p2 = passengerService->createPassenger("Bob");

    // 2. Book the first seat (Success)
    ticketService->bookTicket(t.getTrainId(), p1.getId());

    // 3. Attempt to book the second seat (Should throw Capacity/Full error)
    EXPECT_THROW(ticketService->bookTicket(t.getTrainId(), p2.getId()), std::runtime_error);
}

TEST_F(TicketServiceTest, BookTicket_AllowSamePassengerDifferentTrains) {
    // Verify a passenger can book tickets on multiple different trains
    Train t1 = trainService->createTrain("Train A", 100);
    Train t2 = trainService->createTrain("Train B", 100);
    Passenger p = passengerService->createPassenger("Traveler");

    EXPECT_NO_THROW(ticketService->bookTicket(t1.getTrainId(), p.getId()));
    EXPECT_NO_THROW(ticketService->bookTicket(t2.getTrainId(), p.getId()));
}

TEST_F(TicketServiceTest, GetTicketByTrainAndPassenger_ReturnsCorrectTicket) {
    Train t = trainService->createTrain("SearchTest", 10);
    Passenger p = passengerService->createPassenger("Searcher");

    Ticket created = ticketService->bookTicket(t.getTrainId(), p.getId());

    auto found = ticketService->getTicketByTrainAndPassenger(t.getTrainId(), p.getId());

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->getId(), created.getId());
    EXPECT_EQ(found->getPassenger().getName(), "Searcher");
}

TEST_F(TicketServiceTest, GetTicketByTrainAndPassenger_ReturnsNulloptIfNotFound) {
    Train t = trainService->createTrain("Empty", 10);
    Passenger p = passengerService->createPassenger("Ghost");

    auto found = ticketService->getTicketByTrainAndPassenger(t.getTrainId(), p.getId());

    EXPECT_FALSE(found.has_value());
}

TEST_F(TicketServiceTest, CancelTicket_ReuseSeatStrategy) {
    // Test that a cancelled seat is prioritized for the next booking (LIFO logic in SeatAllocator)
    Train t = trainService->createTrain("StackTest", 5);
    Passenger p1 = passengerService->createPassenger("A"); // Gets Seat 1
    Passenger p2 = passengerService->createPassenger("B"); // Gets Seat 2
    Passenger p3 = passengerService->createPassenger("C"); // Gets Seat 3

    Ticket t1 = ticketService->bookTicket(t.getTrainId(), p1.getId());
    Ticket t2 = ticketService->bookTicket(t.getTrainId(), p2.getId());
    Ticket t3 = ticketService->bookTicket(t.getTrainId(), p3.getId());

    EXPECT_EQ(t2.getSeat(), 2);

    // Cancel Seat 2
    ticketService->cancelTicket(t2.getId());

    // New booking for Passenger D
    Passenger p4 = passengerService->createPassenger("D");
    Ticket t4 = ticketService->bookTicket(t.getTrainId(), p4.getId());

    // Expect D to get seat 2 (the one just cancelled) instead of seat 4
    EXPECT_EQ(t4.getSeat(), 2);
}

TEST_F(TicketServiceTest, BookTicket_ComplexScenario_CancelAndRebook) {
    Train t = trainService->createTrain("Complex", 2);
    Passenger p1 = passengerService->createPassenger("P1");
    Passenger p2 = passengerService->createPassenger("P2");
    Passenger p3 = passengerService->createPassenger("P3");

    // Fill Train
    Ticket tick1 = ticketService->bookTicket(t.getTrainId(), p1.getId());
    ticketService->bookTicket(t.getTrainId(), p2.getId());

    // Try to book P3 (Full)
    EXPECT_THROW(ticketService->bookTicket(t.getTrainId(), p3.getId()), std::runtime_error);

    // Cancel P1
    ticketService->cancelTicket(tick1.getId());

    // Book P3 (Should succeed now)
    EXPECT_NO_THROW(ticketService->bookTicket(t.getTrainId(), p3.getId()));
}