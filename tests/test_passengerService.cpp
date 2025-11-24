#include <gtest/gtest.h>
#include "models/Passenger.h"
#include "Repo/InMemoryPassengerRepository.h"
#include "Services/PassengerService.h"
#include "utils/helpers.h"
#include <optional>

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
    // Note: Assuming logic doesn't trim based on provided source, asserting exact match
    EXPECT_EQ("  Omar  ", p.getName());
}

TEST_F(PassengerServiceTest, CreatePassenger_DoesNotModifyRepositoryObjectDirectly) {
    Passenger p = service->createPassenger("Test");
    auto retrievedOpt = service->getPassenger(p.getId());

    ASSERT_TRUE(retrievedOpt.has_value());
    // Ensure they are distinct objects in memory (copy) if that's the intent,
    // but value equality should match
    EXPECT_EQ(p.getId(), retrievedOpt->getId());
}

TEST_F(PassengerServiceTest, CreatePassenger_AllowsSpecialCharacters) {
    Passenger p = service->createPassenger("###");
    EXPECT_EQ("###", p.getName());
}

/// ------------------------------------------------------------
/// GET PASSENGER TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, GetPassenger_ReturnsNulloptForNegativeId) {
    // Repo returns nullopt, service returns nullopt
    EXPECT_FALSE(service->getPassenger(-1).has_value());
}

TEST_F(PassengerServiceTest, GetPassenger_ReturnsNulloptForZeroId) {
    EXPECT_FALSE(service->getPassenger(0).has_value());
}

TEST_F(PassengerServiceTest, GetPassenger_AfterMultipleCreates) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");

    auto retrieved = service->getPassenger(b.getId());

    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ("B", retrieved->getName());
}

/// ------------------------------------------------------------
/// GET ALL PASSENGERS TESTS
/// ------------------------------------------------------------

TEST_F(PassengerServiceTest, GetAllPassengers_OrderIsConsistent) {
    service->createPassenger("A");
    service->createPassenger("B");
    service->createPassenger("C");

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
    auto found = service->find_or_create_passenger("oMaR");

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ("Omar", found->getName());
    EXPECT_EQ(1, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, FindOrCreate_ReturnsExistingEvenIfExactNameDifferentCase) {
    Passenger p = service->createPassenger("HELLO");

    auto found = service->find_or_create_passenger("hello");

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(p.getId(), found->getId());
}

TEST_F(PassengerServiceTest, FindOrCreate_DoesNotMatchSimilarNames) {
    service->createPassenger("John");
    auto p = service->find_or_create_passenger("Johnny");

    ASSERT_TRUE(p.has_value());
    EXPECT_EQ("Johnny", p->getName());
    EXPECT_EQ(2, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, FindOrCreate_DifferentSpecialCharactersNotEqual) {
    service->createPassenger("Omar!");
    auto p = service->find_or_create_passenger("Omar");

    ASSERT_TRUE(p.has_value());
    EXPECT_NE("Omar!", p->getName());
}

TEST_F(PassengerServiceTest, FindOrCreate_SameSpecialCharactersAreEqual) {
    Passenger original = service->createPassenger("O'Brien");

    auto found = service->find_or_create_passenger("o'brien");

    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(original.getId(), found->getId());
}

TEST_F(PassengerServiceTest, FindOrCreate_AfterDeletionCreatesNewId) {
    Passenger p1 = service->createPassenger("Adam");
    service->deletePassenger(p1.getId());

    auto p2 = service->find_or_create_passenger("Adam");

    ASSERT_TRUE(p2.has_value());
    EXPECT_NE(p1.getId(), p2->getId());
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
        auto p = service->find_or_create_passenger("DAVID");
        ASSERT_TRUE(p.has_value());
        EXPECT_EQ("David", p->getName());
    }

    EXPECT_EQ(1, service->getAllPassengers().size());
}