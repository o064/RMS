//
// Created by Omar on 11/24/2025.
//

#include <gtest/gtest.h>
#include "Services/TicketService.h"
#include "Services/TrainService.h"
#include "Services/PassengerService.h"
#include "Repo/InMemoryTicketRepository .h"
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

TEST_F(TicketServiceTest, BookTicket_HappyPath) {
    // 1. Setup Train and Passenger
    Train t = trainService->createTrain("Express", 10);
    Passenger p = passengerService->createPassenger("Omar");

    // 2. Book Ticket
    Ticket ticket = ticketService->bookTicket(t.getTrainId(), p.getId());

    // 3. Assertions
    EXPECT_GT(ticket.getId(), 0);
    EXPECT_EQ(ticket.getTrainId(), t.getTrainId());
    EXPECT_EQ(ticket.getPassenger().getId(), p.getId());
    EXPECT_EQ(ticket.getStatus(), booked);

    // Check if ticket is in repo
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
    // NOTE: This test will fail unless you fix the logic inversion in TicketService::bookTicket
    // Change: if(!existTicket.has_value()) -> if(existTicket.has_value())

    Train t = trainService->createTrain("Express", 10);
    Passenger p = passengerService->createPassenger("Omar");

    ticketService->bookTicket(t.getTrainId(), p.getId());

    EXPECT_THROW(ticketService->bookTicket(t.getTrainId(), p.getId()), std::runtime_error);
}

TEST_F(TicketServiceTest, BookTicket_AssignsSeatsCorrectly) {
    Train t = trainService->createTrain("Express", 2); // Only 2 seats
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

    // Cancel
    ticketService->cancelTicket(ticket.getId());

    // Verify Status
    auto updatedTicket = ticketService->getTicket(ticket.getId());
    ASSERT_TRUE(updatedTicket.has_value());
    EXPECT_EQ(updatedTicket->getStatus(), cancelled);
}

TEST_F(TicketServiceTest, CancelTicket_ThrowsIfNotFound) {
    EXPECT_THROW(ticketService->cancelTicket(9999), std::runtime_error);
}

TEST_F(TicketServiceTest, CancelTicket_FreesSeat) {
    // NOTE: This test requires TicketService to save the Train state back to repo after modification.
    // If your code doesn't save the train after freeSeat(), this might fail or show old state.

    Train t = trainService->createTrain("Express", 1); // 1 Seat
    Passenger p1 = passengerService->createPassenger("P1");
    Passenger p2 = passengerService->createPassenger("P2");

    // Book the only seat
    Ticket t1 = ticketService->bookTicket(t.getTrainId(), p1.getId());

    // Cancel it
    ticketService->cancelTicket(t1.getId());

    // Should be able to book again for P2
    EXPECT_NO_THROW(ticketService->bookTicket(t.getTrainId(), p2.getId()));
}