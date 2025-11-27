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
    // Repositories
    std::unique_ptr<InMemoryTrainRepository> trainRepo;
    std::unique_ptr<InMemoryPassengerRepository> passengerRepo;
    std::unique_ptr<InMemoryTicketRepository> ticketRepo;

    // Services
    std::unique_ptr<TrainService> trainService;
    std::unique_ptr<PassengerService> passengerService;
    std::unique_ptr<TicketService> ticketService;

    // Facade (System Under Test)
    std::unique_ptr<RMSFacade> rms;

    void SetUp() override {
        // 1. Initialize Repositories
        trainRepo = std::make_unique<InMemoryTrainRepository>();
        passengerRepo = std::make_unique<InMemoryPassengerRepository>();
        ticketRepo = std::make_unique<InMemoryTicketRepository>();

        // Clear any static/residual state
        trainRepo->clear();
        passengerRepo->clear();
        ticketRepo->clear();

        // 2. Initialize Services with Repo pointers
        trainService = std::make_unique<TrainService>(trainRepo.get());
        passengerService = std::make_unique<PassengerService>(passengerRepo.get());

        // TicketService depends on the other two services
        ticketService = std::make_unique<TicketService>(ticketRepo.get(), trainService.get(), passengerService.get());

        // 3. Initialize Facade with Service pointers
        rms = std::make_unique<RMSFacade>(trainService.get(), ticketService.get(), passengerService.get());
    }
};

// ================= TRAIN MANAGEMENT TESTS =================

TEST_F(RMSFacadeTest, AddAndListTrains) {
    rms->addTrain("Express A", 100);
    rms->addTrain("Local B", 50);

    std::vector<Train> trains = rms->listTrains();

    EXPECT_EQ(trains.size(), 2);
    EXPECT_EQ(trains.front().getTrainName(), "Express A");
}

TEST_F(RMSFacadeTest, GetTrain_ReturnsCorrectDetails) {
    Train t = rms->addTrain("Bullet Train", 200);

    auto fetched = rms->getTrain(t.getTrainId());

    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched->getTrainName(), "Bullet Train");
    EXPECT_EQ(fetched->getTotalSeats(), 200);
}

TEST_F(RMSFacadeTest, GetTrainAvailability_CorrectlyReportsStatus) {
    Train t = rms->addTrain("Empty Train", 10);
    EXPECT_TRUE(rms->getTrainAvailability(t.getTrainId()));
}

// ================= PASSENGER MANAGEMENT TESTS =================

TEST_F(RMSFacadeTest, AddAndListPassengers) {
    rms->addPassenger("Alice");
    rms->addPassenger("Bob");

    auto passengers = rms->listPassengers();

    EXPECT_EQ(passengers.size(), 2);
}

TEST_F(RMSFacadeTest, GetPassenger_FoundAndNotFound) {
    Passenger p = rms->addPassenger("Charlie");

    // Found
    auto found = rms->getPassenger(p.getId());
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->getName(), "Charlie");

    // Not Found
    auto notFound = rms->getPassenger(999);
    EXPECT_FALSE(notFound.has_value());
}

// ================= BOOKING FLOW TESTS (INTEGRATION) =================
TEST_F(RMSFacadeTest, BookTicket_HappyPath) {
    // 1. Setup
    Train t = rms->addTrain("City Connector", 50);
    // Passenger is added explicitly to get ID for assertion, but Facade uses the name
    Passenger p = rms->addPassenger("David");

    // 2. Action: Use passenger's name for booking
    Ticket ticket = rms->bookTicket(t.getTrainId(), "David");

    // 3. Assert
    EXPECT_GT(ticket.getId(), 0);
    EXPECT_EQ(ticket.getTrainId(), t.getTrainId());
    // Verify the ticket was issued to the expected passenger ID
    EXPECT_EQ(ticket.getPassenger().getId(), p.getId());

    // Verify it appears in the list
    auto allTickets = rms->listTickets();
    EXPECT_EQ(allTickets.size(), 1);
}

TEST_F(RMSFacadeTest, BookTicket_UpdatesSeatAvailability) {
    // 1. Create a train with EXACTLY 1 seat
    Train t = rms->addTrain("Tiny Train", 1);

    // 2. Book the only seat using passenger's name
    rms->bookTicket(t.getTrainId(), "Eve");

    // 3. Verify availability via Facade
    EXPECT_FALSE(rms->getTrainAvailability(t.getTrainId()));

    // 4. Verify second booking fails (using a different passenger's name)
    EXPECT_THROW(rms->bookTicket(t.getTrainId(), "Frank"), std::runtime_error);
}

TEST_F(RMSFacadeTest, BookTicket_ValidatesInputs) {
    // Invalid Train ID (999 does not exist)
    EXPECT_THROW(rms->bookTicket(999, "David"), std::runtime_error);

    Train t = rms->addTrain("Valid Train", 10);
    // Invalid Passenger Name (Assuming empty string is invalid input)
    EXPECT_THROW(rms->bookTicket(t.getTrainId(), ""), std::runtime_error);
}
TEST_F(RMSFacadeTest, CancelTicket_RestoresAvailability) {
    // 1. Setup Train with 1 seat
    Train t = rms->addTrain("Full Train", 1);
    Passenger p1 = rms->addPassenger("Gary");
    Passenger p2 = rms->addPassenger("Helen");

    // 2. Fill the train using name
    Ticket ticket = rms->bookTicket(t.getTrainId(), "Gary"); // Using updated signature
    EXPECT_FALSE(rms->getTrainAvailability(t.getTrainId()));

    // 3. Cancel the ticket
    rms->cancelTicket(ticket.getId());

    // 4. Check if ticket status changed
    // Facade doesn't have getTicket(id), so we check the list (assuming 'cancelled' is a valid enum/status value)
    auto tickets = rms->listTickets();
    // Assuming tickets.front() is the cancelled ticket if only one exists/is checked
    // A better implementation would iterate or fetch by ID if possible. We use front for simplicity here.
    EXPECT_EQ(tickets.front().getStatus(), cancelled);

    // 5. Verify we can book again (Seat freed) using name
    EXPECT_TRUE(rms->getTrainAvailability(t.getTrainId()));
    EXPECT_NO_THROW(rms->bookTicket(t.getTrainId(), "Helen")); // Using updated signature
}
TEST_F(RMSFacadeTest, FullWorkflow_Scenario) {
    // 1. Admin sets up system
    Train express = rms->addTrain("Hogwarts Express", 100);
    // Explicitly add passengers for consistency and verification
    Passenger harry = rms->addPassenger("Harry");
    Passenger ron = rms->addPassenger("Ron");

    // 2. Users book tickets using names
    Ticket t1 = rms->bookTicket(express.getTrainId(), "Harry");
    Ticket t2 = rms->bookTicket(express.getTrainId(), "Ron");

    // 3. Verify Manifest
    auto tickets = rms->listTickets();
    EXPECT_EQ(tickets.size(), 2);

    // 4. Ron cancels
    rms->cancelTicket(t2.getId());

    // 5. Verify cancellation logic (assuming 'booked' and 'cancelled' are valid status values)
    int activeCount = 0;
    int cancelledCount = 0;
    for(const auto& t : rms->listTickets()) {
        if(t.getStatus() == booked) activeCount++;
        if(t.getStatus() == cancelled) cancelledCount++;
    }

    EXPECT_EQ(activeCount, 1);
    EXPECT_EQ(cancelledCount, 1);
}