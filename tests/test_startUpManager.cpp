#include <gtest/gtest.h>
#include "StartupManager.h"
#include "RMSFacade.h"

class StartupManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<StartupManager> manager;
    RMSFacade* facade;

    void SetUp() override {
        // Initialize the manager
        manager = std::make_unique<StartupManager>();

        // Build the system and get the facade pointer
        facade = manager->buildFacade();
    }

    void TearDown() override {
        // Manager destructor handles the cleanup of Facade, Services, and Repos
        manager.reset();
    }
};

// 1. Basic Construction Test
TEST_F(StartupManagerTest, BuildFacade_ReturnsValidPointer) {
    EXPECT_NE(facade, nullptr);
}

// 2. Verify Wiring: Train Service -> Repo
TEST_F(StartupManagerTest, Facade_HasWorkingTrainService) {
    // Try to use the facade to ensure the underlying TrainService is initialized
    facade->addTrain("Startup Express", 100);

    auto trains = facade->listTrains();
    EXPECT_EQ(trains.size(), 1);
    EXPECT_EQ(trains.front().getTrainName(), "Startup Express");
}

// 3. Verify Wiring: Passenger Service -> Repo
TEST_F(StartupManagerTest, Facade_HasWorkingPassengerService) {
    facade->addPassenger("Test User");

    auto passengers = facade->listPassenger();
    EXPECT_EQ(passengers.size(), 1);
    EXPECT_EQ(passengers.front().getName(), "Test User");
}

// 4. Verify Wiring: Ticket Service -> TrainService + PassengerService
TEST_F(StartupManagerTest, Facade_IntegrationIsWiredCorrectly) {
    // This tests the deepest part of the graph:
    // Facade -> TicketService -> (Repo + TrainService + PassengerService)

    Train t = facade->addTrain("Integration Train", 10);
    Passenger p = facade->addPassenger("Integration Passenger");

    // If pointers were null, this would crash
    Ticket ticket = facade->bookTicket(t.getTrainId(), p.getId());

    EXPECT_GT(ticket.getId(), 0);

    auto allTickets = facade->listTickets();
    EXPECT_EQ(allTickets.size(), 1);
}

// 5. Verify Lifecycle / Reset Behavior
TEST_F(StartupManagerTest, BuildFacade_ResetsApplicationState) {
    // Add data to the current instance
    facade->addTrain("Data to be lost", 10);
    EXPECT_EQ(facade->listTrains().size(), 1);

    // Call buildFacade again on the same manager
    // Based on your implementation, this re-assigns the unique_ptrs,
    // which deletes the old repos and creates new empty ones.
    RMSFacade* newFacade = manager->buildFacade();

    // Verify pointer is still valid
    EXPECT_NE(newFacade, nullptr);

    // Verify data is GONE (Empty state)
    EXPECT_EQ(newFacade->listTrains().size(), 0);
}