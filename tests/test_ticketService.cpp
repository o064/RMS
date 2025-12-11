#include <gtest/gtest.h>
#include "Services/TicketService.h"
#include "Services/TrainService.h"
#include "Services/PassengerService.h"
#include "Repo/InMemoryTicketRepository.h"
#include "Repo/InMemoryTrainRepository.h"
#include "Repo/InMemoryPassengerRepository.h"

class TicketServiceTest : public ::testing::Test {
protected:
    std::unique_ptr<InMemoryTicketRepository> ticketRepo;
    std::unique_ptr<InMemoryTrainRepository> trainRepo;
    std::unique_ptr<InMemoryPassengerRepository> passengerRepo;

    std::unique_ptr<TrainService> trainService;
    std::unique_ptr<PassengerService> passengerService;
    std::unique_ptr<TicketService> ticketService;

    void SetUp() override {
        ticketRepo = std::make_unique<InMemoryTicketRepository>();
        trainRepo = std::make_unique<InMemoryTrainRepository>();
        passengerRepo = std::make_unique<InMemoryPassengerRepository>();

        trainService = std::make_unique<TrainService>(trainRepo.get());
        passengerService = std::make_unique<PassengerService>(passengerRepo.get());
        ticketService = std::make_unique<TicketService>(
                ticketRepo.get(),
                trainService.get(),
                passengerService.get()
        );
    }

    void TearDown() override {
        ticketRepo->clear();
        trainRepo->clear();
        passengerRepo->clear();
    }
};

// ===================== Book Ticket Tests =====================

TEST_F(TicketServiceTest, BookTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());

    ASSERT_TRUE(ticket.has_value());
    EXPECT_GT(ticket->getId(), 0);
    EXPECT_EQ(ticket->getTrainId(), train.getTrainId());
    EXPECT_EQ(ticket->getPassenger().getId(), passenger.getId());
    EXPECT_EQ(ticket->getStatus(), booked);
    EXPECT_GT(ticket->getSeat(), 0);
}

TEST_F(TicketServiceTest, BookTicket_TrainNotFound) {
    Passenger passenger = passengerService->createPassenger("John");
    EXPECT_THROW(ticketService->bookTicket(999, passenger.getId()), std::out_of_range);
}

TEST_F(TicketServiceTest, BookTicket_PassengerNotFound) {
    Train train = trainService->createTrain("Express", 10);
    EXPECT_THROW(ticketService->bookTicket(train.getTrainId(), 999), std::out_of_range);
}

TEST_F(TicketServiceTest, BookTicket_DuplicateBooking) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    ticketService->bookTicket(train.getTrainId(), passenger.getId());
    EXPECT_THROW(ticketService->bookTicket(train.getTrainId(), passenger.getId()), std::runtime_error);
}

TEST_F(TicketServiceTest, BookTicket_FullTrainReturnsNullopt) {
    Train train = trainService->createTrain("Express", 2);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");
    Passenger p3 = passengerService->createPassenger("Bob");

    ticketService->bookTicket(train.getTrainId(), p1.getId());
    ticketService->bookTicket(train.getTrainId(), p2.getId());

    auto ticket = ticketService->bookTicket(train.getTrainId(), p3.getId());
    EXPECT_FALSE(ticket.has_value());
}

// ===================== Cancel Ticket Tests =====================

TEST_F(TicketServiceTest, CancelTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ASSERT_TRUE(ticket.has_value());

    EXPECT_NO_THROW(ticketService->cancelTicket(ticket->getId()));

    auto retrieved = ticketService->getTicket(ticket->getId());
    EXPECT_EQ(retrieved.getStatus(), cancelled);
}

TEST_F(TicketServiceTest, CancelTicket_AlreadyCancelledThrows) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ticketService->cancelTicket(ticket->getId());

    EXPECT_THROW(ticketService->cancelTicket(ticket->getId()), std::runtime_error);
}

// ===================== Get Ticket Tests =====================

TEST_F(TicketServiceTest, GetTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto booked = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    auto retrieved = ticketService->getTicket(booked->getId());

    EXPECT_EQ(retrieved.getId(), booked->getId());
}

TEST_F(TicketServiceTest, GetTicket_NotFoundThrows) {
    EXPECT_THROW(ticketService->getTicket(999), std::out_of_range);
}

// ===================== Update Ticket Tests =====================

TEST_F(TicketServiceTest, UpdateTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    Ticket modified = ticket.value();
    modified.setStatus(cancelled);

    Ticket updated = ticketService->updateTicket(modified);
    EXPECT_EQ(updated.getStatus(), cancelled);
}

// ===================== Waiting List Integration =====================

TEST_F(TicketServiceTest, CancelTicket_FreesSeatAndRebooksWaitingPassenger) {
    Train train = trainService->createTrain("Express", 1);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());

    EXPECT_FALSE(t2.has_value()); // fully booked

    ticketService->cancelTicket(t1->getId()); // should auto-assign seat to p2

    auto allTickets = ticketService->getAllTickets();
    int bookedCount = 0;
    for (auto& t : allTickets) {
        if (t.getStatus() == booked) bookedCount++;
    }
    EXPECT_EQ(bookedCount, 1);
}
