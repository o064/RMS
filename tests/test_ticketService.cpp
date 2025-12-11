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

// ===================== Basic Booking Tests =====================

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

    EXPECT_THROW(
            ticketService->bookTicket(999, passenger.getId()),
            std::out_of_range
    );
}

TEST_F(TicketServiceTest, BookTicket_PassengerNotFound) {
    Train train = trainService->createTrain("Express", 10);

    EXPECT_THROW(
            ticketService->bookTicket(train.getTrainId(), 999),
            std::runtime_error
    );
}

TEST_F(TicketServiceTest, BookTicket_DuplicateBooking) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    ticketService->bookTicket(train.getTrainId(), passenger.getId());

    EXPECT_THROW(
            ticketService->bookTicket(train.getTrainId(), passenger.getId()),
            std::runtime_error
    );
}

TEST_F(TicketServiceTest, BookTicket_FullTrain_ReturnsNullopt) {
    Train train = trainService->createTrain("Express", 2);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");
    Passenger p3 = passengerService->createPassenger("Bob");

    ticketService->bookTicket(train.getTrainId(), p1.getId());
    ticketService->bookTicket(train.getTrainId(), p2.getId());

    auto ticket = ticketService->bookTicket(train.getTrainId(), p3.getId());
    EXPECT_FALSE(ticket.has_value());
}

TEST_F(TicketServiceTest, BookTicket_SequentialSeats) {
    Train train = trainService->createTrain("Express", 10);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");
    Passenger p3 = passengerService->createPassenger("Bob");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());
    auto t3 = ticketService->bookTicket(train.getTrainId(), p3.getId());

    ASSERT_TRUE(t1.has_value());
    ASSERT_TRUE(t2.has_value());
    ASSERT_TRUE(t3.has_value());

    EXPECT_EQ(t1->getSeat(), 1);
    EXPECT_EQ(t2->getSeat(), 2);
    EXPECT_EQ(t3->getSeat(), 3);
}

// ===================== Cancel Ticket Tests =====================

TEST_F(TicketServiceTest, CancelTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ASSERT_TRUE(ticket.has_value());

    EXPECT_NO_THROW(ticketService->cancelTicket(ticket->getId()));

    auto retrieved = ticketService->getTicket(ticket->getId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getStatus(), cancelled);
}

TEST_F(TicketServiceTest, CancelTicket_NotFound) {
    EXPECT_THROW(
            ticketService->cancelTicket(999),
            std::runtime_error
    );
}

TEST_F(TicketServiceTest, CancelTicket_AlreadyCancelled) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ASSERT_TRUE(ticket.has_value());

    ticketService->cancelTicket(ticket->getId());

    EXPECT_THROW(
            ticketService->cancelTicket(ticket->getId()),
            std::runtime_error
    );
}

TEST_F(TicketServiceTest, CancelTicket_FreesSeats) {
    Train train = trainService->createTrain("Express", 2);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());

    ASSERT_TRUE(t1.has_value());
    ASSERT_TRUE(t2.has_value());

    auto updated = trainService->getTrain(train.getTrainId());
    ASSERT_TRUE(updated.has_value());
    EXPECT_FALSE(updated->hasAvailableSeats());

    ticketService->cancelTicket(t1->getId());

    updated = trainService->getTrain(train.getTrainId());
    ASSERT_TRUE(updated.has_value());
    EXPECT_TRUE(updated->hasAvailableSeats());
}

TEST_F(TicketServiceTest, CancelTicket_AssignsToWaitingList) {
    Train train = trainService->createTrain("Express", 2);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");
    Passenger p3 = passengerService->createPassenger("Bob");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());
    ticketService->bookTicket(train.getTrainId(), p3.getId());

    ASSERT_TRUE(t1.has_value());

    ticketService->cancelTicket(t1->getId());

    auto tickets = ticketService->getAllTickets();
    int bookedCount = 0;
    for (const auto& ticket : tickets) {
        if (ticket.getStatus() == booked) bookedCount++;
    }
    EXPECT_EQ(bookedCount, 2);
}

// ===================== Get Ticket Tests =====================

TEST_F(TicketServiceTest, GetTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto booked = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ASSERT_TRUE(booked.has_value());

    auto retrieved = ticketService->getTicket(booked->getId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getId(), booked->getId());
}

TEST_F(TicketServiceTest, GetTicket_NotFound) {
    EXPECT_THROW(ticketService->getTicket(999), std::runtime_error);
}

TEST_F(TicketServiceTest, GetTicketByTrainAndPassenger_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto booked = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ASSERT_TRUE(booked.has_value());

    auto retrieved = ticketService->getTicketByTrainAndPassenger(
            train.getTrainId(),
            passenger.getId()
    );

    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getId(), booked->getId());
}

TEST_F(TicketServiceTest, GetTicketByTrainAndPassenger_NotFound) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto retrieved = ticketService->getTicketByTrainAndPassenger(
            train.getTrainId(),
            passenger.getId()
    );

    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(TicketServiceTest, GetAllTickets_Empty) {
    auto tickets = ticketService->getAllTickets();
    EXPECT_TRUE(tickets.empty());
}

TEST_F(TicketServiceTest, GetAllTickets_Multiple) {
    Train train = trainService->createTrain("Express", 10);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");
    Passenger p3 = passengerService->createPassenger("Bob");

    ticketService->bookTicket(train.getTrainId(), p1.getId());
    ticketService->bookTicket(train.getTrainId(), p2.getId());
    ticketService->bookTicket(train.getTrainId(), p3.getId());

    auto tickets = ticketService->getAllTickets();
    EXPECT_EQ(tickets.size(), 3);
}

// ===================== Update Ticket Tests =====================

TEST_F(TicketServiceTest, UpdateTicket_Success) {
    Train train = trainService->createTrain("Express", 10);
    Passenger passenger = passengerService->createPassenger("John");

    auto ticket = ticketService->bookTicket(train.getTrainId(), passenger.getId());
    ASSERT_TRUE(ticket.has_value());

    Ticket modified = ticket.value();
    modified.setStatus(cancelled);

    Ticket updated = ticketService->updateTicket(modified);
    EXPECT_EQ(updated.getStatus(), cancelled);
}

// ===================== Integration Tests =====================

TEST_F(TicketServiceTest, CompleteBookingCancellationFlow) {
    Train train = trainService->createTrain("Express", 5);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());

    ASSERT_TRUE(t1.has_value());
    ASSERT_TRUE(t2.has_value());

    auto allTickets = ticketService->getAllTickets();
    EXPECT_EQ(allTickets.size(), 2);

    ticketService->cancelTicket(t1->getId());

    auto retrieved = ticketService->getTicket(t1->getId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getStatus(), cancelled);
}

TEST_F(TicketServiceTest, MultipleTrainsMultiplePassengers) {
    Train t1 = trainService->createTrain("Express", 10);
    Train t2 = trainService->createTrain("Local", 10);

    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");

    auto ticket1 = ticketService->bookTicket(t1.getTrainId(), p1.getId());
    auto ticket2 = ticketService->bookTicket(t1.getTrainId(), p2.getId());
    auto ticket3 = ticketService->bookTicket(t2.getTrainId(), p1.getId());
    auto ticket4 = ticketService->bookTicket(t2.getTrainId(), p2.getId());

    ASSERT_TRUE(ticket1.has_value());
    ASSERT_TRUE(ticket2.has_value());
    ASSERT_TRUE(ticket3.has_value());
    ASSERT_TRUE(ticket4.has_value());

    auto allTickets = ticketService->getAllTickets();
    EXPECT_EQ(allTickets.size(), 4);
}

TEST_F(TicketServiceTest, WaitingListIntegration) {
    Train train = trainService->createTrain("Express", 1);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    ASSERT_TRUE(t1.has_value());

    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());
    EXPECT_FALSE(t2.has_value());

    ticketService->cancelTicket(t1->getId());

    auto allTickets = ticketService->getAllTickets();
    int bookedCount = 0;
    for (const auto& ticket : allTickets) {
        if (ticket.getStatus() == booked) bookedCount++;
    }
    EXPECT_EQ(bookedCount, 1);
}

// ===================== Edge Cases =====================

TEST_F(TicketServiceTest, BookAllSeatsInTrain) {
    Train train = trainService->createTrain("Express", 3);

    vector<Passenger> passengers;
    for (int i = 0; i < 3; i++) {
        passengers.push_back(
                passengerService->createPassenger("Passenger" + std::to_string(i))
        );
    }

    for (const auto& p : passengers) {
        auto ticket = ticketService->bookTicket(train.getTrainId(), p.getId());
        ASSERT_TRUE(ticket.has_value());
    }

    auto updated = trainService->getTrain(train.getTrainId());
    ASSERT_TRUE(updated.has_value());
    EXPECT_FALSE(updated->hasAvailableSeats());
}

TEST_F(TicketServiceTest, CancelAndRebook) {
    Train train = trainService->createTrain("Express", 2);
    Passenger p1 = passengerService->createPassenger("John");
    Passenger p2 = passengerService->createPassenger("Jane");

    auto t1 = ticketService->bookTicket(train.getTrainId(), p1.getId());
    ASSERT_TRUE(t1.has_value());

    ticketService->cancelTicket(t1->getId());

    auto t2 = ticketService->bookTicket(train.getTrainId(), p2.getId());
    ASSERT_TRUE(t2.has_value());
    EXPECT_EQ(t2->getSeat(), t1->getSeat());
}