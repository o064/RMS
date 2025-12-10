#include <gtest/gtest.h>
#include "RMSFacade.h"
#include "Services/TrainService.h"
#include "Services/PassengerService.h"
#include "Services/TicketService.h"
#include "Repo/InMemoryTrainRepository.h"
#include "Repo/InMemoryPassengerRepository.h"
#include "Repo/InMemoryTicketRepository.h"

class RMSFacadeTest : public ::testing::Test {
protected:
    std::unique_ptr<InMemoryTrainRepository> trainRepo;
    std::unique_ptr<InMemoryPassengerRepository> passengerRepo;
    std::unique_ptr<InMemoryTicketRepository> ticketRepo;

    std::unique_ptr<TrainService> trainService;
    std::unique_ptr<PassengerService> passengerService;
    std::unique_ptr<TicketService> ticketService;

    std::unique_ptr<RMSFacade> facade;

    void SetUp() override {
        trainRepo = std::make_unique<InMemoryTrainRepository>();
        passengerRepo = std::make_unique<InMemoryPassengerRepository>();
        ticketRepo = std::make_unique<InMemoryTicketRepository>();

        trainService = std::make_unique<TrainService>(trainRepo.get());
        passengerService = std::make_unique<PassengerService>(passengerRepo.get());
        ticketService = std::make_unique<TicketService>(
                ticketRepo.get(),
                trainService.get(),
                passengerService.get()
        );

        facade = std::make_unique<RMSFacade>(
                trainService.get(),
                ticketService.get(),
                passengerService.get()
        );
    }

    void TearDown() override {
        trainRepo->clear();
        passengerRepo->clear();
        ticketRepo->clear();
    }
};

// ===================== Train Operations =====================

TEST_F(RMSFacadeTest, AddTrain_Success) {
    Train train = facade->addTrain("Express", 20);

    EXPECT_GT(train.getTrainId(), 0);
    EXPECT_EQ(train.getTrainName(), "Express");
    EXPECT_EQ(train.getTotalSeats(), 20);
}

TEST_F(RMSFacadeTest, AddTrain_EmptyName) {
    EXPECT_THROW(facade->addTrain("", 20), std::invalid_argument);
}

TEST_F(RMSFacadeTest, AddTrain_WhitespaceName) {
    EXPECT_THROW(facade->addTrain("   ", 20), std::invalid_argument);
}

TEST_F(RMSFacadeTest, AddTrain_InvalidSeats) {
    EXPECT_THROW(facade->addTrain("Express", 0), std::invalid_argument);
    EXPECT_THROW(facade->addTrain("Express", -5), std::invalid_argument);
}

TEST_F(RMSFacadeTest, AddTrain_TrimsName) {
    Train train = facade->addTrain("  Express  ", 20);
    EXPECT_EQ(train.getTrainName(), "Express");
}

TEST_F(RMSFacadeTest, ListTrains_Empty) {
    auto trains = facade->listTrains();
    EXPECT_TRUE(trains.empty());
}

TEST_F(RMSFacadeTest, ListTrains_Multiple) {
    facade->addTrain("Express", 20);
    facade->addTrain("Local", 30);
    facade->addTrain("Regional", 15);

    auto trains = facade->listTrains();
    EXPECT_EQ(trains.size(), 3);
}

TEST_F(RMSFacadeTest, GetTrain_Success) {
    Train added = facade->addTrain("Express", 20);
    auto retrieved = facade->getTrain(added.getTrainId());

    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getTrainName(), "Express");
}

TEST_F(RMSFacadeTest, GetTrain_NotFound) {
    EXPECT_THROW(facade->getTrain(999), std::out_of_range);
}

TEST_F(RMSFacadeTest, UpdateTrain_Success) {
    Train train = facade->addTrain("Old Name", 10);
    Train updated = facade->updateTrain(train.getTrainId(), "New Name", 15);

    EXPECT_EQ(updated.getTrainName(), "New Name");
    EXPECT_EQ(updated.getTotalSeats(), 15);
}

TEST_F(RMSFacadeTest, UpdateTrain_InvalidId) {
    EXPECT_THROW(
            facade->updateTrain(0, "Test", 10),
            std::invalid_argument
    );
}

TEST_F(RMSFacadeTest, UpdateTrain_NotFound) {
    EXPECT_THROW(
            facade->updateTrain(999, "Test", 10),
            std::out_of_range
    );
}

TEST_F(RMSFacadeTest, UpdateTrain_EmptyName) {
    Train train = facade->addTrain("Test", 10);
    EXPECT_THROW(
            facade->updateTrain(train.getTrainId(), "", 10),
            std::invalid_argument
    );
}

TEST_F(RMSFacadeTest, UpdateTrain_NegativeSeats) {
    Train train = facade->addTrain("Test", 10);
    EXPECT_THROW(
            facade->updateTrain(train.getTrainId(), "Test", -5),
            std::invalid_argument
    );
}

TEST_F(RMSFacadeTest, UpdateTrain_ProcessesWaitingList) {
    Train train = facade->addTrain("Test", 2);
    facade->addPassenger("John");
    facade->addPassenger("Jane");
    facade->addPassenger("Bob");

    facade->bookTicket(train.getTrainId(), "John");
    facade->bookTicket(train.getTrainId(), "Jane");
    facade->bookTicket(train.getTrainId(), "Bob");

    Train updated = facade->updateTrain(train.getTrainId(), "Test", 5);

    auto tickets = facade->listTickets();
    int bookedCount = 0;
    for (const auto& ticket : tickets) {
        if (ticket.getStatus() == booked) bookedCount++;
    }
    EXPECT_EQ(bookedCount, 3);
}

TEST_F(RMSFacadeTest, AddSeats_ById) {
    Train train = facade->addTrain("Test", 10);
    Train updated = facade->addSeats(train.getTrainId(), 5);

    EXPECT_EQ(updated.getTotalSeats(), 15);
}

TEST_F(RMSFacadeTest, AddSeats_ByName) {
    facade->addTrain("Test Train", 10);
    Train updated = facade->addSeats("Test Train", 5);

    EXPECT_EQ(updated.getTotalSeats(), 15);
}

TEST_F(RMSFacadeTest, DeleteTrain_Success) {
    Train train = facade->addTrain("Test", 10);
    EXPECT_NO_THROW(facade->deleteTrain(train.getTrainId()));
}

TEST_F(RMSFacadeTest, DeleteTrain_NotFound) {
    EXPECT_THROW(facade->deleteTrain(999), std::out_of_range);
}

TEST_F(RMSFacadeTest, GetTrainAvailability_Available) {
    Train train = facade->addTrain("Test", 10);
    EXPECT_TRUE(facade->getTrainAvailability(train.getTrainId()));
}

TEST_F(RMSFacadeTest, GetTrainAvailability_Full) {
    Train train = facade->addTrain("Test", 1);
    facade->addPassenger("John");
    facade->bookTicket(train.getTrainId(), "John");

    EXPECT_FALSE(facade->getTrainAvailability(train.getTrainId()));
}

// ===================== Passenger Operations =====================

TEST_F(RMSFacadeTest, AddPassenger_Success) {
    Passenger p = facade->addPassenger("John Doe");

    EXPECT_GT(p.getId(), 0);
    EXPECT_EQ(p.getName(), "John Doe");
}

TEST_F(RMSFacadeTest, AddPassenger_EmptyName) {
    EXPECT_THROW(facade->addPassenger(""), std::invalid_argument);
}

TEST_F(RMSFacadeTest, AddPassenger_WhitespaceName) {
    EXPECT_THROW(facade->addPassenger("   "), std::invalid_argument);
}

TEST_F(RMSFacadeTest, AddPassenger_TrimsName) {
    Passenger p = facade->addPassenger("  John  ");
    EXPECT_EQ(p.getName(), "John");
}

TEST_F(RMSFacadeTest, ListPassengers_Empty) {
    auto passengers = facade->listPassengers();
    EXPECT_TRUE(passengers.empty());
}

TEST_F(RMSFacadeTest, ListPassengers_Multiple) {
    facade->addPassenger("John");
    facade->addPassenger("Jane");
    facade->addPassenger("Bob");

    auto passengers = facade->listPassengers();
    EXPECT_EQ(passengers.size(), 3);
}

TEST_F(RMSFacadeTest, GetPassenger_Success) {
    Passenger added = facade->addPassenger("John");
    auto retrieved = facade->getPassenger(added.getId());

    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getName(), "John");
}

TEST_F(RMSFacadeTest, UpdatePassenger_Success) {
    Passenger p = facade->addPassenger("Old Name");
    Passenger updated = facade->updatePassenger(p.getId(), "New Name");

    EXPECT_EQ(updated.getName(), "New Name");
}

TEST_F(RMSFacadeTest, UpdatePassenger_NotFound) {
    EXPECT_THROW(
            facade->updatePassenger(999, "Test"),
            std::out_of_range
    );
}

TEST_F(RMSFacadeTest, UpdatePassenger_UpdatesTickets) {
    Train train = facade->addTrain("Test", 10);
    Passenger p = facade->addPassenger("John");

    facade->bookTicket(train.getTrainId(), "John");
    facade->updatePassenger(p.getId(), "John Updated");

    auto tickets = facade->listTickets();
    EXPECT_EQ(tickets[0].getPassenger().getName(), "John Updated");
}

TEST_F(RMSFacadeTest, DeletePassenger_Success) {
    Passenger p = facade->addPassenger("John");
    EXPECT_NO_THROW(facade->deletePassenger(p.getId()));
}

TEST_F(RMSFacadeTest, DeletePassenger_NotFound) {
    EXPECT_THROW(facade->deletePassenger(999), std::out_of_range);
}

// ===================== Ticket Operations =====================

TEST_F(RMSFacadeTest, BookTicket_Success) {
    Train train = facade->addTrain("Express", 10);
    facade->addPassenger("John");

    auto ticket = facade->bookTicket(train.getTrainId(), "John");

    ASSERT_TRUE(ticket.has_value());
    EXPECT_EQ(ticket->getTrainId(), train.getTrainId());
    EXPECT_EQ(ticket->getPassenger().getName(), "John");
}

TEST_F(RMSFacadeTest, BookTicket_CreatesPassengerIfNotExists) {
    Train train = facade->addTrain("Express", 10);

    auto ticket = facade->bookTicket(train.getTrainId(), "New Passenger");

    ASSERT_TRUE(ticket.has_value());
    EXPECT_EQ(ticket->getPassenger().getName(), "New Passenger");

    auto passengers = facade->listPassengers();
    EXPECT_EQ(passengers.size(), 1);
}

TEST_F(RMSFacadeTest, BookTicket_InvalidTrainId) {
    EXPECT_THROW(
            facade->bookTicket(0, "John"),
            std::invalid_argument
    );
}

TEST_F(RMSFacadeTest, BookTicket_EmptyPassengerName) {
    Train train = facade->addTrain("Express", 10);
    EXPECT_THROW(
            facade->bookTicket(train.getTrainId(), ""),
            std::invalid_argument
    );
}

TEST_F(RMSFacadeTest, BookTicket_TrimsPassengerName) {
    Train train = facade->addTrain("Express", 10);

    auto ticket = facade->bookTicket(train.getTrainId(), "  John  ");

    ASSERT_TRUE(ticket.has_value());
    EXPECT_EQ(ticket->getPassenger().getName(), "John");
}

TEST_F(RMSFacadeTest, BookTicket_FullTrain) {
    Train train = facade->addTrain("Express", 1);

    facade->bookTicket(train.getTrainId(), "John");
    auto ticket2 = facade->bookTicket(train.getTrainId(), "Jane");

    EXPECT_FALSE(ticket2.has_value());
}

TEST_F(RMSFacadeTest, CancelTicket_Success) {
    Train train = facade->addTrain("Express", 10);
    auto ticket = facade->bookTicket(train.getTrainId(), "John");

    ASSERT_TRUE(ticket.has_value());
    EXPECT_NO_THROW(facade->cancelTicket(ticket->getId()));
}

TEST_F(RMSFacadeTest, CancelTicket_InvalidId) {
    EXPECT_THROW(facade->cancelTicket(0), std::invalid_argument);
}

TEST_F(RMSFacadeTest, ListTickets_Empty) {
    auto tickets = facade->listTickets();
    EXPECT_TRUE(tickets.empty());
}

TEST_F(RMSFacadeTest, ListTickets_Multiple) {
    Train train = facade->addTrain("Express", 10);

    facade->bookTicket(train.getTrainId(), "John");
    facade->bookTicket(train.getTrainId(), "Jane");
    facade->bookTicket(train.getTrainId(), "Bob");

    auto tickets = facade->listTickets();
    EXPECT_EQ(tickets.size(), 3);
}

// ===================== Integration Tests =====================

TEST_F(RMSFacadeTest, CompleteWorkflow) {
    Train train = facade->addTrain("Express", 10);
    Passenger p1 = facade->addPassenger("John");
    Passenger p2 = facade->addPassenger("Jane");

    auto ticket1 = facade->bookTicket(train.getTrainId(), "John");
    auto ticket2 = facade->bookTicket(train.getTrainId(), "Jane");

    ASSERT_TRUE(ticket1.has_value());
    ASSERT_TRUE(ticket2.has_value());

    auto tickets = facade->listTickets();
    EXPECT_EQ(tickets.size(), 2);

    facade->cancelTicket(ticket1->getId());

    auto updatedTickets = facade->listTickets();
    int bookedCount = 0;
    for (const auto& t : updatedTickets) {
        if (t.getStatus() == booked) bookedCount++;
    }
    EXPECT_EQ(bookedCount, 1);
}

TEST_F(RMSFacadeTest, MultipleTrainsMultiplePassengers) {
    Train t1 = facade->addTrain("Express", 10);
    Train t2 = facade->addTrain("Local", 15);

    facade->addPassenger("John");
    facade->addPassenger("Jane");

    facade->bookTicket(t1.getTrainId(), "John");
    facade->bookTicket(t1.getTrainId(), "Jane");
    facade->bookTicket(t2.getTrainId(), "John");

    auto trains = facade->listTrains();
    auto passengers = facade->listPassengers();
    auto tickets = facade->listTickets();

    EXPECT_EQ(trains.size(), 2);
    EXPECT_EQ(passengers.size(), 2);
    EXPECT_EQ(tickets.size(), 3);
}

TEST_F(RMSFacadeTest, WaitingListIntegration) {
    Train train = facade->addTrain("Express", 1);

    auto ticket1 = facade->bookTicket(train.getTrainId(), "John");
    auto ticket2 = facade->bookTicket(train.getTrainId(), "Jane");

    ASSERT_TRUE(ticket1.has_value());
    EXPECT_FALSE(ticket2.has_value());

    facade->cancelTicket(ticket1->getId());

    auto tickets = facade->listTickets();
    int bookedCount = 0;
    for (const auto& t : tickets) {
        if (t.getStatus() == booked) bookedCount++;
    }
    EXPECT_EQ(bookedCount, 1);
}

TEST_F(RMSFacadeTest, UpdateTrainExpandsCapacity) {
    Train train = facade->addTrain("Test", 5);

    for (int i = 0; i < 5; i++) {
        facade->bookTicket(train.getTrainId(), "Passenger" + std::to_string(i));
    }

    facade->bookTicket(train.getTrainId(), "Waiting1");
    facade->bookTicket(train.getTrainId(), "Waiting2");

    facade->updateTrain(train.getTrainId(), "Test", 10);

    auto tickets = facade->listTickets();
    int bookedCount = 0;
    for (const auto& t : tickets) {
        if (t.getStatus() == booked) bookedCount++;
    }
    EXPECT_GE(bookedCount, 6);
}