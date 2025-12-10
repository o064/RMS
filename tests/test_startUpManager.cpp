#include <gtest/gtest.h>
#include "StartupManager.h"
#include "RMSFacade.h"

class StartupManagerTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ===================== Basic Tests =====================

TEST_F(StartupManagerTest, BuildFacade_ReturnsValidPointer) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    ASSERT_NE(facade, nullptr);
}

TEST_F(StartupManagerTest, BuildFacade_InitializesComponents) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    ASSERT_NE(facade, nullptr);

    // Test that facade can perform operations
    EXPECT_NO_THROW({
                        auto trains = facade->listTrains();
                        auto passengers = facade->listPassengers();
                        auto tickets = facade->listTickets();
                    });
}

TEST_F(StartupManagerTest, BuildFacade_LoadsMockData) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    ASSERT_NE(facade, nullptr);

    auto trains = facade->listTrains();
    auto passengers = facade->listPassengers();
    auto tickets = facade->listTickets();

    // Mock data loads 5 trains, 10 passengers, and 10+ tickets
    EXPECT_EQ(trains.size(), 5);
    EXPECT_EQ(passengers.size(), 10);
    EXPECT_GE(tickets.size(), 10);
}

// ===================== Mock Data Verification =====================

TEST_F(StartupManagerTest, MockData_TrainsExist) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();

    std::vector<std::string> expectedTrains = {
            "Alex NightLine",
            "Luxor Premium",
            "Cairo Express",
            "Aswan Sleeper",
            "Mediterranean Star"
    };

    EXPECT_EQ(trains.size(), expectedTrains.size());

    for (const auto& expectedName : expectedTrains) {
        bool found = false;
        for (const auto& train : trains) {
            if (train.getTrainName() == expectedName) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Train not found: " << expectedName;
    }
}

TEST_F(StartupManagerTest, MockData_PassengersExist) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto passengers = facade->listPassengers();

    std::vector<std::string> expectedPassengers = {
            "Omar", "Sara", "Mohamed", "Nour", "Ali",
            "Mona", "Youssef", "Laila", "Karim", "Dina"
    };

    EXPECT_EQ(passengers.size(), expectedPassengers.size());

    for (const auto& expectedName : expectedPassengers) {
        bool found = false;
        for (const auto& passenger : passengers) {
            if (passenger.getName() == expectedName) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found) << "Passenger not found: " << expectedName;
    }
}

TEST_F(StartupManagerTest, MockData_TicketsExist) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto tickets = facade->listTickets();

    // Based on loadMockData, should have at least 11 tickets
    // (some passengers like Omar book multiple tickets)
    EXPECT_GE(tickets.size(), 10);

    // Verify all tickets are booked status
    for (const auto& ticket : tickets) {
        EXPECT_EQ(ticket.getStatus(), booked);
    }
}

TEST_F(StartupManagerTest, MockData_TrainsHaveCorrectCapacity) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();

    std::map<std::string, int> expectedCapacities = {
            {"Alex NightLine", 30},
            {"Luxor Premium", 40},
            {"Cairo Express", 25},
            {"Aswan Sleeper", 20},
            {"Mediterranean Star", 35}
    };

    for (const auto& train : trains) {
        auto it = expectedCapacities.find(train.getTrainName());
        if (it != expectedCapacities.end()) {
            EXPECT_EQ(train.getTotalSeats(), it->second)
                                << "Train " << train.getTrainName()
                                << " has incorrect capacity";
        }
    }
}

// ===================== Facade Functionality Tests =====================

TEST_F(StartupManagerTest, Facade_CanAddNewTrain) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    Train newTrain = facade->addTrain("New Train", 50);

    EXPECT_GT(newTrain.getTrainId(), 0);
    EXPECT_EQ(newTrain.getTrainName(), "New Train");
    EXPECT_EQ(newTrain.getTotalSeats(), 50);

    auto trains = facade->listTrains();
    EXPECT_EQ(trains.size(), 6); // 5 mock + 1 new
}

TEST_F(StartupManagerTest, Facade_CanAddNewPassenger) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    Passenger newPassenger = facade->addPassenger("New Passenger");

    EXPECT_GT(newPassenger.getId(), 0);
    EXPECT_EQ(newPassenger.getName(), "New Passenger");

    auto passengers = facade->listPassengers();
    EXPECT_EQ(passengers.size(), 11); // 10 mock + 1 new
}

TEST_F(StartupManagerTest, Facade_CanBookNewTicket) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    auto ticket = facade->bookTicket(trains[0].getTrainId(), "Test Passenger");

    ASSERT_TRUE(ticket.has_value());
    EXPECT_EQ(ticket->getTrainId(), trains[0].getTrainId());
}

TEST_F(StartupManagerTest, Facade_CanCancelTicket) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto tickets = facade->listTickets();
    ASSERT_FALSE(tickets.empty());

    EXPECT_NO_THROW(facade->cancelTicket(tickets[0].getId()));
}

TEST_F(StartupManagerTest, Facade_CanUpdateTrain) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    Train updated = facade->updateTrain(
            trains[0].getTrainId(),
            "Updated Name",
            50
    );

    EXPECT_EQ(updated.getTrainName(), "Updated Name");
    EXPECT_EQ(updated.getTotalSeats(), 50);
}

TEST_F(StartupManagerTest, Facade_CanUpdatePassenger) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto passengers = facade->listPassengers();
    ASSERT_FALSE(passengers.empty());

    Passenger updated = facade->updatePassenger(
            passengers[0].getId(),
            "Updated Name"
    );

    EXPECT_EQ(updated.getName(), "Updated Name");
}

TEST_F(StartupManagerTest, Facade_CanDeleteTrain) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());
    int initialCount = trains.size();

    EXPECT_NO_THROW(facade->deleteTrain(trains[0].getTrainId()));

    auto updatedTrains = facade->listTrains();
    EXPECT_EQ(updatedTrains.size(), initialCount - 1);
}

TEST_F(StartupManagerTest, Facade_CanDeletePassenger) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto passengers = facade->listPassengers();
    ASSERT_FALSE(passengers.empty());
    int initialCount = passengers.size();

    // Find a passenger without tickets to delete
    Passenger toDelete = facade->addPassenger("Temp Passenger");

    EXPECT_NO_THROW(facade->deletePassenger(toDelete.getId()));

    auto updatedPassengers = facade->listPassengers();
    EXPECT_EQ(updatedPassengers.size(), initialCount);
}

// ===================== Integration Tests =====================

TEST_F(StartupManagerTest, MultipleOperationsOnFacade) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    // Add new entities
    Train newTrain = facade->addTrain("Integration Train", 20);
    Passenger newPassenger = facade->addPassenger("Integration Passenger");

    // Book ticket
    auto ticket = facade->bookTicket(
            newTrain.getTrainId(),
            "Integration Passenger"
    );

    ASSERT_TRUE(ticket.has_value());

    // Verify all lists
    auto trains = facade->listTrains();
    auto passengers = facade->listPassengers();
    auto tickets = facade->listTickets();

    EXPECT_GE(trains.size(), 6);
    EXPECT_GE(passengers.size(), 11);
    EXPECT_GE(tickets.size(), 11);
}

TEST_F(StartupManagerTest, FacadeStateConsistency) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    // Get initial state
    auto initialTrains = facade->listTrains();
    auto initialPassengers = facade->listPassengers();
    auto initialTickets = facade->listTickets();

    // Perform operations
    Train train = facade->addTrain("Test Train", 10);
    facade->addPassenger("Test Passenger");
    facade->bookTicket(train.getTrainId(), "Test Passenger");

    // Verify state changed appropriately
    auto finalTrains = facade->listTrains();
    auto finalPassengers = facade->listPassengers();
    auto finalTickets = facade->listTickets();

    EXPECT_EQ(finalTrains.size(), initialTrains.size() + 1);
    EXPECT_EQ(finalPassengers.size(), initialPassengers.size() + 1);
    EXPECT_EQ(finalTickets.size(), initialTickets.size() + 1);
}

// ===================== Memory Management Tests =====================

TEST_F(StartupManagerTest, MultipleStartupManagers) {
    {
        StartupManager manager1;
        RMSFacade* facade1 = manager1.buildFacade();
        ASSERT_NE(facade1, nullptr);

        auto trains1 = facade1->listTrains();
        EXPECT_EQ(trains1.size(), 5);
    }

    {
        StartupManager manager2;
        RMSFacade* facade2 = manager2.buildFacade();
        ASSERT_NE(facade2, nullptr);

        auto trains2 = facade2->listTrains();
        EXPECT_EQ(trains2.size(), 5);
    }
}

TEST_F(StartupManagerTest, FacadeLifetime) {
    RMSFacade* facade;
    {
        StartupManager manager;
        facade = manager.buildFacade();
        ASSERT_NE(facade, nullptr);

        // Facade should be valid within StartupManager scope
        auto trains = facade->listTrains();
        EXPECT_EQ(trains.size(), 5);
    }
    // After StartupManager goes out of scope, facade pointer is invalid
    // Don't access facade here - it would be undefined behavior
}

// ===================== Edge Cases =====================

TEST_F(StartupManagerTest, BookTicketToMockTrain) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();
    ASSERT_FALSE(trains.empty());

    auto ticket = facade->bookTicket(trains[0].getTrainId(), "New Passenger");

    ASSERT_TRUE(ticket.has_value());
    EXPECT_EQ(ticket->getTrainId(), trains[0].getTrainId());
}

TEST_F(StartupManagerTest, AllMockTrainsHaveAvailability) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto trains = facade->listTrains();

    for (const auto& train : trains) {
        bool availability = facade->getTrainAvailability(train.getTrainId());
        EXPECT_TRUE(availability)
                            << "Train " << train.getTrainName()
                            << " should have availability";
    }
}

TEST_F(StartupManagerTest, MockPassengersHaveTickets) {
    StartupManager manager;
    RMSFacade* facade = manager.buildFacade();

    auto passengers = facade->listPassengers();
    auto tickets = facade->listTickets();

    // Verify that some passengers have tickets
    std::set<int> passengerIdsWithTickets;
    for (const auto& ticket : tickets) {
        passengerIdsWithTickets.insert(ticket.getPassenger().getId());
    }

    EXPECT_FALSE(passengerIdsWithTickets.empty());
}