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

TEST_F(RMSFacadeTest, AddTrain_Invalid) {
    EXPECT_THROW(facade->addTrain("", 20), std::invalid_argument);
    EXPECT_THROW(facade->addTrain("   ", 20), std::invalid_argument);
    EXPECT_THROW(facade->addTrain("Express", 0), std::invalid_argument);
    EXPECT_THROW(facade->addTrain("Express", -5), std::invalid_argument);
}

TEST_F(RMSFacadeTest, ListTrains_Multiple) {
    facade->addTrain("Express", 20);
    facade->addTrain("Local", 30);
    auto trains = facade->listTrains();
    EXPECT_EQ(trains.size(), 2);
}

TEST_F(RMSFacadeTest, GetTrain_SuccessAndNotFound) {
    Train added = facade->addTrain("Express", 20);
    auto retrieved = facade->getTrain(added.getTrainId());
    EXPECT_EQ(retrieved.getTrainName(), "Express");
    EXPECT_THROW(facade->getTrain(999), std::out_of_range);
}

TEST_F(RMSFacadeTest, UpdateTrain_SuccessAndEdgeCases) {
    Train train = facade->addTrain("Old Name", 10);
    Train updated = facade->updateTrain(train.getTrainId(), "New Name", 15);
    EXPECT_EQ(updated.getTrainName(), "New Name");
    EXPECT_EQ(updated.getTotalSeats(), 15);
    EXPECT_THROW(facade->updateTrain(0, "Test", 10), std::invalid_argument);
    EXPECT_THROW(facade->updateTrain(train.getTrainId(), "", 10), std::invalid_argument);
    EXPECT_THROW(facade->updateTrain(train.getTrainId(), "Test", -5), std::invalid_argument);
    EXPECT_THROW(facade->updateTrain(999, "Test", 10), std::out_of_range);
}

TEST_F(RMSFacadeTest, AddSeatsByIdAndName) {
    Train train = facade->addTrain("Test", 10);
    Train updatedById = facade->addSeats(train.getTrainId(), 5);
    EXPECT_EQ(updatedById.getTotalSeats(), 15);

    facade->addTrain("Test Train", 10);
    Train updatedByName = facade->addSeats("Test Train", 5);
    EXPECT_EQ(updatedByName.getTotalSeats(), 15);
}

TEST_F(RMSFacadeTest, DeleteTrain_SuccessAndNotFound) {
    Train train = facade->addTrain("Test", 10);
    EXPECT_NO_THROW(facade->deleteTrain(train.getTrainId()));
    EXPECT_THROW(facade->deleteTrain(999), std::out_of_range);
}

TEST_F(RMSFacadeTest, GetTrainAvailability) {
    Train train = facade->addTrain("Test", 1);
    EXPECT_TRUE(facade->getTrainAvailability(train.getTrainId()));
    facade->addPassenger("John");
    facade->bookTicket(train.getTrainId(), "John");
    EXPECT_FALSE(facade->getTrainAvailability(train.getTrainId()));
}

// ===================== Passenger Operations =====================

TEST_F(RMSFacadeTest, AddPassenger_SuccessAndInvalid) {
    Passenger p = facade->addPassenger("John Doe");
    EXPECT_GT(p.getId(), 0);
    EXPECT_EQ(p.getName(), "John Doe");

    EXPECT_THROW(facade->addPassenger(""), std::invalid_argument);
    EXPECT_THROW(facade->addPassenger("   "), std::invalid_argument);
}

TEST_F(RMSFacadeTest, UpdatePassenger_SuccessAndTickets) {
    Train train = facade->addTrain("Test", 10);
    Passenger p = facade->addPassenger("John");
    facade->bookTicket(train.getTrainId(), "John");
    EXPECT_THROW( facade->bookTicket(train.getTrainId(), "John"), std::runtime_error);

    Passenger updated = facade->updatePassenger(p.getId(), "John Updated");

    EXPECT_EQ(updated.getName(), "John Updated");
    auto tickets = facade->listTickets();
    for (auto& t : tickets) {
        if (t.getPassenger().getId() == p.getId())
            EXPECT_EQ(t.getPassenger().getName(), "John Updated");
    }

    EXPECT_THROW(facade->updatePassenger(999, "Test"), std::out_of_range);
}

TEST_F(RMSFacadeTest, DeletePassenger_SuccessAndNotFound) {
    Passenger p = facade->addPassenger("John");
    EXPECT_NO_THROW(facade->deletePassenger(p.getId()));
    EXPECT_THROW(facade->deletePassenger(999), std::out_of_range);
}

// ===================== Ticket Operations =====================

TEST_F(RMSFacadeTest, BookTicket_SuccessAndEdgeCases) {
    Train train = facade->addTrain("Express", 3);
    facade->addPassenger("John");

    auto ticket = facade->bookTicket(train.getTrainId(), "John");
    ASSERT_TRUE(ticket.has_value());
    EXPECT_EQ(ticket->getPassenger().getName(), "John");

    // passenger auto-created
    auto ticket2 = facade->bookTicket(train.getTrainId(), "New Passenger");
    ASSERT_TRUE(ticket2.has_value());
    EXPECT_EQ(ticket2->getPassenger().getName(), "New Passenger");

    EXPECT_THROW(facade->bookTicket(0, "John"), std::invalid_argument);
    EXPECT_THROW(facade->bookTicket(train.getTrainId(), ""), std::invalid_argument);
    auto ticket3 = facade->bookTicket(train.getTrainId(), "   Trim Test   ");
    ASSERT_TRUE(ticket3.has_value());
    EXPECT_EQ(ticket3->getPassenger().getName(), "Trim Test");
}


// ===================== Integration Tests =====================

TEST_F(RMSFacadeTest, CompleteWorkflowAndWaitingList) {
    Train train = facade->addTrain("Express", 1);
    auto t1 = facade->bookTicket(train.getTrainId(), "John");
    auto t2 = facade->bookTicket(train.getTrainId(), "Jane");
    EXPECT_FALSE(t2.has_value());

    facade->cancelTicket(t1->getId());

    auto tickets = facade->listTickets();
    int bookedCount = 0;
    for (const auto& t : tickets) if (t.getStatus() == booked) bookedCount++;
    EXPECT_EQ(bookedCount, 1);
}

TEST_F(RMSFacadeTest, UpdateTrainExpandsCapacityAndFulfillsWaitingList) {
    Train train = facade->addTrain("Test", 2);
    facade->bookTicket(train.getTrainId(), "Passenger1");
    facade->bookTicket(train.getTrainId(), "Passenger2");
    facade->bookTicket(train.getTrainId(), "Waiting1");
    facade->bookTicket(train.getTrainId(), "Waiting2");

    facade->updateTrain(train.getTrainId(), "Test", 5);

    auto tickets = facade->listTickets();
    int bookedCount = 0;
    for (const auto& t : tickets) if (t.getStatus() == booked) bookedCount++;
    EXPECT_GE(bookedCount, 4); // 2 waiting passengers booked after expansion
}
