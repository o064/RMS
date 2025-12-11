#include <gtest/gtest.h>
#include "models/Passenger.h"
#include "Repo/InMemoryPassengerRepository.h"
#include "Services/PassengerService.h"
#include "utils/helpers.h"
#include <set>

class PassengerServiceTest : public ::testing::Test {
protected:
    InMemoryPassengerRepository repo;
    PassengerService* service;

    void SetUp() override {
        repo.clear();
        service = new PassengerService(&repo);
    }

    void TearDown() override {
        delete service;
        repo.clear();
    }
};

//
// CREATE PASSENGER TESTS
//

TEST_F(PassengerServiceTest, CreatePassenger_AssignsUniqueIds) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");
    EXPECT_NE(a.getId(), b.getId());
}

TEST_F(PassengerServiceTest, CreatePassenger_TrimsName) {
    Passenger p = service->createPassenger("  Omar  ");
    EXPECT_EQ("Omar", p.getName());
}

TEST_F(PassengerServiceTest, CreatePassenger_EmptyNameThrows) {
    EXPECT_THROW(service->createPassenger(""), std::invalid_argument);
}

//
// GET PASSENGER TESTS
//

TEST_F(PassengerServiceTest, GetPassenger_ThrowsForNegativeId) {
    EXPECT_THROW(service->getPassenger(-1), std::out_of_range);
}

TEST_F(PassengerServiceTest, GetPassenger_ReturnsCorrectPassenger) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");

    Passenger retrieved = service->getPassenger(b.getId());
    EXPECT_EQ("B", retrieved.getName());
}

TEST_F(PassengerServiceTest, GetPassenger_ThrowsForNonExistentId) {
    service->createPassenger("A");
    EXPECT_THROW(service->getPassenger(999), std::out_of_range);
}

//
// GET ALL PASSENGERS
//

TEST_F(PassengerServiceTest, GetAllPassengers_SizeCorrect) {
    service->createPassenger("A");
    service->createPassenger("B");
    service->createPassenger("C");

    auto all = service->getAllPassengers();
    EXPECT_EQ(3, all.size());
}

//
// DELETE PASSENGER
//
TEST_F(PassengerServiceTest, DeletePassenger_RemovesOnlyTarget) {
    Passenger p1 = service->createPassenger("A");
    Passenger p2 = service->createPassenger("B");

    service->deletePassenger(p1.getId());  // throws on failure

    auto all = service->getAllPassengers();
    EXPECT_EQ(1, all.size());
    EXPECT_EQ("B", all[0].getName());
}

TEST_F(PassengerServiceTest, DeletePassenger_ThrowsForNonExistent) {
    EXPECT_THROW(service->deletePassenger(999), std::out_of_range);
}

TEST_F(PassengerServiceTest, DeletePassenger_SuccessDoesNotThrow) {
    Passenger p = service->createPassenger("A");
    EXPECT_NO_THROW(service->deletePassenger(p.getId()));
}

//
// UPDATE PASSENGER
//

TEST_F(PassengerServiceTest, UpdatePassenger_Success) {
    Passenger p = service->createPassenger("Original");
    int id = p.getId();

    Passenger updated = service->updatePassenger(id, "Updated");

    EXPECT_EQ(id, updated.getId());
    EXPECT_EQ("Updated", updated.getName());
}

TEST_F(PassengerServiceTest, UpdatePassenger_ThrowsForNonExistent) {
    EXPECT_THROW(service->updatePassenger(999, "Name"), std::out_of_range);
}

//
// FIND OR CREATE PASSENGER
//

TEST_F(PassengerServiceTest, FindOrCreate_FindsExistingIgnoringCase) {
    service->createPassenger("Omar");

    Passenger found = service->find_or_create_passenger("oMaR");

    EXPECT_EQ("Omar", found.getName());
    EXPECT_EQ(1, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, FindOrCreate_ReturnsExistingEvenIfCaseDiffers) {
    Passenger p = service->createPassenger("HELLO");

    Passenger found = service->find_or_create_passenger("hello");

    EXPECT_EQ(p.getId(), found.getId());
    EXPECT_EQ("HELLO", found.getName());
}

TEST_F(PassengerServiceTest, FindOrCreate_DoesNotMatchDifferentNames) {
    service->createPassenger("John");

    Passenger p = service->find_or_create_passenger("Johnny");

    EXPECT_EQ("Johnny", p.getName());
    EXPECT_EQ(2, service->getAllPassengers().size());
}



TEST_F(PassengerServiceTest, Create100Passengers_AllIdsUnique) {
    std::set<int> ids;

    for (int i = 0; i < 100; i++) {
        Passenger p = service->createPassenger("P" + std::to_string(i));
        ids.insert(p.getId());
    }

    EXPECT_EQ(100, ids.size());
}
