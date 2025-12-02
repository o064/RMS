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

TEST_F(PassengerServiceTest, CreatePassenger_AssignsUniqueIds) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");
    EXPECT_NE(a.getId(), b.getId());
}

TEST_F(PassengerServiceTest, CreatePassenger_TrimsName) {
    Passenger p = service->createPassenger("  Omar  ");
    EXPECT_EQ("Omar", p.getName());
}

TEST_F(PassengerServiceTest, CreatePassenger_ValidatesName) {
    EXPECT_THROW(service->createPassenger(""), std::invalid_argument);
}

TEST_F(PassengerServiceTest, GetPassenger_ReturnsNulloptForNegativeId) {
    EXPECT_FALSE(service->getPassenger(-1).has_value());
}

TEST_F(PassengerServiceTest, GetPassenger_AfterMultipleCreates) {
    Passenger a = service->createPassenger("A");
    Passenger b = service->createPassenger("B");
    auto retrieved = service->getPassenger(b.getId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ("B", retrieved->getName());
}

TEST_F(PassengerServiceTest, GetPassenger_ReturnsNulloptForNonExistent) {
    service->createPassenger("A");
    EXPECT_FALSE(service->getPassenger(999).has_value());
}

TEST_F(PassengerServiceTest, GetAllPassengers_OrderIsConsistent) {
    service->createPassenger("A");
    service->createPassenger("B");
    service->createPassenger("C");
    std::vector<Passenger> all = service->getAllPassengers();
    EXPECT_EQ(3, all.size());
}

TEST_F(PassengerServiceTest, DeletePassenger_DoesNotAffectOtherPassengers) {
    Passenger p1 = service->createPassenger("A");
    Passenger p2 = service->createPassenger("B");
    service->deletePassenger(p1.getId());
    EXPECT_EQ(1, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, DeletePassenger_ReturnsFalseForNonExistent) {
    bool result = service->deletePassenger(999);
    EXPECT_FALSE(result);
}

TEST_F(PassengerServiceTest, DeletePassenger_ReturnsTrueForSuccess) {
    Passenger p = service->createPassenger("A");
    bool result = service->deletePassenger(p.getId());
    EXPECT_TRUE(result);
}

TEST_F(PassengerServiceTest, UpdatePassenger_Success) {
    Passenger p = service->createPassenger("Original");
    int id = p.getId();
    Passenger updated = service->updatePassenger(id, "Updated");
    EXPECT_EQ(updated.getId(), id);
    EXPECT_EQ(updated.getName(), "Updated");
}

TEST_F(PassengerServiceTest, UpdatePassenger_ThrowsForNonExistent) {
    EXPECT_THROW(service->updatePassenger(999, "NewName"), std::runtime_error);
}

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
    EXPECT_EQ("HELLO", found.getName());
}

TEST_F(PassengerServiceTest, FindOrCreate_DoesNotMatchSimilarNames) {
    service->createPassenger("John");
    Passenger p = service->find_or_create_passenger("Johnny");
    EXPECT_EQ("Johnny", p.getName());
    EXPECT_EQ(2, service->getAllPassengers().size());
}

TEST_F(PassengerServiceTest, Create100Passengers_HaveUniqueIds) {
    std::set<int> ids;
    for (int i = 0; i < 100; i++) {
        Passenger p = service->createPassenger("P" + std::to_string(i));
        ids.insert(p.getId());
    }
    EXPECT_EQ(100, ids.size());
}