#include <gtest/gtest.h>
#include "models/Passenger.h"
#include "Repo/InMemoryPassengerRepository.h"
#include <optional>

class InMemoryPassengerRepositoryTest : public ::testing::Test {
protected:
    InMemoryPassengerRepository repo;

    void SetUp() override {
        repo.clear();
    }

    void TearDown() override {
        repo.clear();
    }
};

TEST_F(InMemoryPassengerRepositoryTest, SavePassengerWithZeroIdGeneratesNewId) {
    Passenger p(0, "Alice");
    repo.save(p);
    EXPECT_GT(p.getId(), 0);
}

TEST_F(InMemoryPassengerRepositoryTest, SaveGeneratesSequentialIds) {
    Passenger p1(0, "Alice");
    Passenger p2(0, "Bob");
    repo.save(p1);
    repo.save(p2);
    EXPECT_EQ(1, p1.getId());
    EXPECT_EQ(2, p2.getId());
}

TEST_F(InMemoryPassengerRepositoryTest, SavePassengerWithExistingIdPreservesId) {
    Passenger p(42, "Charlie");
    repo.save(p);
    EXPECT_EQ(42, p.getId());
}

TEST_F(InMemoryPassengerRepositoryTest, SaveUpdatesExistingPassenger) {
    Passenger p(0, "David");
    repo.save(p);
    int originalId = p.getId();
    Passenger updated(originalId, "David Updated");
    repo.save(updated);
    auto retrieved = repo.getPassenger(originalId);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(originalId, retrieved->getId());
    EXPECT_EQ("David Updated", retrieved->getName());
}

TEST_F(InMemoryPassengerRepositoryTest, SaveMultipleTimesWithSameId) {
    Passenger p1(5, "Eve");
    repo.save(p1);
    Passenger p2(5, "Eve Updated");
    repo.save(p2);
    auto retrieved = repo.getPassenger(5);
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ("Eve Updated", retrieved->getName());
    EXPECT_EQ(1, repo.getAllPassengers().size());
}

TEST_F(InMemoryPassengerRepositoryTest, GetPassengerReturnsCorrectPassenger) {
    Passenger p(0, "Frank");
    repo.save(p);
    auto retrieved = repo.getPassenger(p.getId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(p.getId(), retrieved->getId());
    EXPECT_EQ("Frank", retrieved->getName());
}

TEST_F(InMemoryPassengerRepositoryTest, GetPassengerReturnsNulloptWhenNotFound) {
    auto result = repo.getPassenger(999);
    EXPECT_FALSE(result.has_value());
}

TEST_F(InMemoryPassengerRepositoryTest, GetAllPassengersReturnsEmptyWhenEmpty) {
    std::vector<Passenger> passengers = repo.getAllPassengers();
    EXPECT_TRUE(passengers.empty());
}

TEST_F(InMemoryPassengerRepositoryTest, GetAllPassengersReturnsAllSavedPassengers) {
    Passenger p1(0, "Jack");
    Passenger p2(0, "Kate");
    Passenger p3(0, "Liam");
    repo.save(p1);
    repo.save(p2);
    repo.save(p3);
    std::vector<Passenger> passengers = repo.getAllPassengers();
    EXPECT_EQ(3, passengers.size());
}

TEST_F(InMemoryPassengerRepositoryTest, DeletePassengerRemovesPassenger) {
    Passenger p(0, "Olivia");
    repo.save(p);
    int id = p.getId();
    bool deleted = repo.deletePassenger(id);
    EXPECT_TRUE(deleted);
    auto result = repo.getPassenger(id);
    EXPECT_FALSE(result.has_value());
}

TEST_F(InMemoryPassengerRepositoryTest, DeletePassengerReturnsFalseWhenNotFound) {
    bool deleted = repo.deletePassenger(999);
    EXPECT_FALSE(deleted);
}

TEST_F(InMemoryPassengerRepositoryTest, DeletePassengerReducesCount) {
    Passenger p1(0, "Paul");
    Passenger p2(0, "Quinn");
    repo.save(p1);
    repo.save(p2);
    EXPECT_EQ(2, repo.getAllPassengers().size());
    repo.deletePassenger(p1.getId());
    EXPECT_EQ(1, repo.getAllPassengers().size());
}

TEST_F(InMemoryPassengerRepositoryTest, ClearRemovesAllPassengers) {
    Passenger p1(0, "Uma");
    Passenger p2(0, "Victor");
    repo.save(p1);
    repo.save(p2);
    repo.clear();
    std::vector<Passenger> passengers = repo.getAllPassengers();
    EXPECT_TRUE(passengers.empty());
}

TEST_F(InMemoryPassengerRepositoryTest, ClearResetsNextId) {
    Passenger p1(0, "Wendy");
    repo.save(p1);
    repo.clear();
    Passenger p2(0, "Xavier");
    repo.save(p2);
    EXPECT_EQ(1, p2.getId());
}