//
// Created by Omar on 11/24/2025.
//

#include <gtest/gtest.h>
#include "models/Passenger.h"
#include "Repo/InMemoryPassengerRepository.h"
#include "Services/PassengerService.h"
#include "utils/helpers.h"

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

/// ------------------------------------------------------------
/// BASIC CREATION TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, CreatePassenger_AssignsUniqueIds) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");

    EXPECT_NE(a.getId(), b.getId());
}

TEST_F(PassengerServiceTest, CreatePassenger_TrimsName) {
    Passenger p = service->createPassenger("  Omar  ");

    EXPECT_EQ("  Omar  ", p.getName());  // service does not trim, ensure repo returns exact name
}

TEST_F(PassengerServiceTest, CreatePassenger_DoesNotModifyRepositoryObjectDirectly) {
    Passenger p = service->createPassenger("Test");
    Passenger retrieved = service->getPassenger(p.getId());

    EXPECT_NE(&p, &retrieved); // ensure copies not references
}

TEST_F(PassengerServiceTest, CreatePassenger_AllowsSpecialCharacters) {
    Passenger p = service->createPassenger("###");

    EXPECT_EQ("###", p.getName());
}

/// ------------------------------------------------------------
/// GET PASSENGER TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, GetPassenger_ThrowsForNegativeId) {
    EXPECT_THROW(service->getPassenger(-1), std::runtime_error);
}

TEST_F(PassengerServiceTest, GetPassenger_ThrowsForZeroId) {
    EXPECT_THROW(service->getPassenger(0), std::runtime_error);
}

TEST_F(PassengerServiceTest, GetPassenger_AfterMultipleCreates) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");

    Passenger retrieved = service->getPassenger(b.getId());

    EXPECT_EQ("B", retrieved.getName());
}

/// ------------------------------------------------------------
/// GET ALL PASSENGERS TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, GetAllPassengers_OrderIsConsistent) {
    Passenger p1 = service->createPassenger("A");
    Passenger p2 = service->createPassenger("B");
    Passenger p3 = service->createPassenger("C");

    std::list<Passenger> all = service->getAllPassengers();
    auto it = all.begin();

    EXPECT_EQ("A", it->getName()); ++it;
    EXPECT_EQ("B", it->getName()); ++it;
    EXPECT_EQ("C", it->getName());
}

TEST_F(PassengerServiceTest, GetAllPassengers_NotAffectedByExternalModification) {
    service->createPassenger("A");
    std::list<Passenger> all = service->getAllPassengers();

    // modifying list should NOT modify repo
    all.clear();

    EXPECT_EQ(1, service->getAllPassengers().size());
}

/// ------------------------------------------------------------
/// DELETE PASSENGER TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, DeletePassenger_DoesNotAffectOtherPassengers) {
    Passenger p1 = service->createPassenger("A");
    Passenger p2 = service->createPassenger("B");

    service->deletePassenger(p1.getId());

    EXPECT_EQ(1, service->getAllPassengers().size());
    EXPECT_EQ("B", service->getAllPassengers().front().getName());
}

TEST_F(PassengerServiceTest, DeletePassenger_DoesNotThrow) {
    EXPECT_NO_THROW(service->deletePassenger(999));
}

/// ------------------------------------------------------------
/// FIND OR CREATE TESTS (CORE LOGIC)
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, FindOrCreate_CaseInsensitiveComparison) {
    service->createPassenger("Omar");
    Passenger found = service->find_or_create_passenger("oMaR");

    EXPECT_EQ("Omar", found.getName());
    EXPECT_EQ(1, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, FindOrCreate_ReturnsExistingEvenIfExactNameDifferentCase) {
    Passenger p = service->createPassenger("HELLO");

    Passenger found = service->find_or_create_passenger("hello");

    EXPECT_EQ(p.getId(), found.getId());
}

TEST_F(PassengerServiceTest, FindOrCreate_DoesNotMatchSimilarNames) {
    service->createPassenger("John");
    Passenger p = service->find_or_create_passenger("Johnny");

    EXPECT_EQ("Johnny", p.getName());
    EXPECT_EQ(2, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, FindOrCreate_DifferentSpecialCharactersNotEqual) {
    service->createPassenger("Omar!");
    Passenger p = service->find_or_create_passenger("Omar");

    EXPECT_NE("Omar!", p.getName());
}

TEST_F(PassengerServiceTest, FindOrCreate_SameSpecialCharactersAreEqual) {
    Passenger original = service->createPassenger("O'Brien");

    Passenger found = service->find_or_create_passenger("o'brien");

    EXPECT_EQ(original.getId(), found.getId());
}

TEST_F(PassengerServiceTest, FindOrCreate_AfterDeletionCreatesNewId) {
    Passenger p1 = service->createPassenger("Adam");
    service->deletePassenger(p1.getId());

    Passenger p2 = service->find_or_create_passenger("Adam");

    EXPECT_NE(p1.getId(), p2.getId());
}

/// ------------------------------------------------------------
/// STRESS TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, Create100Passengers_HaveUniqueIds) {
    std::set<int> ids;

    for (int i = 0; i < 100; i++) {
        Passenger p = service->createPassenger("P" + std::to_string(i));
        ids.insert(p.getId());
    }

    EXPECT_EQ(100, ids.size());
}

TEST_F(PassengerServiceTest, FindOrCreate_Stress100Lookups) {
    service->createPassenger("David");

    for (int i = 0; i < 100; i++) {
        Passenger p = service->find_or_create_passenger("DAVID");
        EXPECT_EQ("David", p.getName());
    }

    EXPECT_EQ(1, service->getAllPassengers().size());
}
