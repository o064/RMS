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

    std::vector<Passenger> all = repo.getAllPassengers();
    EXPECT_EQ(1, all.size());
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
    // Implementation returns nullopt, does not throw
    auto result = repo.getPassenger(999);
    EXPECT_FALSE(result.has_value());
}

TEST_F(InMemoryPassengerRepositoryTest, GetPassengerAfterMultipleSaves) {
    Passenger p1(0, "Grace");
    Passenger p2(0, "Henry");
    Passenger p3(0, "Ivan");

    repo.save(p1);
    repo.save(p2);
    repo.save(p3);

    auto retrieved = repo.getPassenger(p2.getId());
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ("Henry", retrieved->getName());
}

TEST_F(InMemoryPassengerRepositoryTest, GetAllPassengersReturnsEmptyvectorWhenEmpty) {
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

TEST_F(InMemoryPassengerRepositoryTest, GetAllPassengersContainsCorrectData) {
    Passenger p1(0, "Mia");
    Passenger p2(0, "Noah");

    repo.save(p1);
    repo.save(p2);

    std::vector<Passenger> passengers = repo.getAllPassengers();

    bool foundMia = false;
    bool foundNoah = false;

    for (const auto& p : passengers) {
        if (p.getName() == "Mia") foundMia = true;
        if (p.getName() == "Noah") foundNoah = true;
    }

    EXPECT_TRUE(foundMia);
    EXPECT_TRUE(foundNoah);
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

TEST_F(InMemoryPassengerRepositoryTest, DeleteAllPassengersOneByOne) {
    Passenger p1(0, "Rachel");
    Passenger p2(0, "Sam");
    Passenger p3(0, "Tina");

    repo.save(p1);
    repo.save(p2);
    repo.save(p3);

    repo.deletePassenger(p1.getId());
    repo.deletePassenger(p2.getId());
    repo.deletePassenger(p3.getId());

    EXPECT_TRUE(repo.getAllPassengers().empty());
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

TEST_F(InMemoryPassengerRepositoryTest, ClearCanBeCalledMultipleTimes) {
    repo.clear();
    repo.clear();
    EXPECT_TRUE(repo.getAllPassengers().empty());
}

TEST_F(InMemoryPassengerRepositoryTest, SaveWithHighIdUpdatesNextId) {
    Passenger p1(100, "Yara");
    repo.save(p1);

    Passenger p2(0, "Zara");
    repo.save(p2);

    EXPECT_EQ(101, p2.getId());
}

TEST_F(InMemoryPassengerRepositoryTest, SaveWithMultipleHighIds) {
    Passenger p1(50, "Adam");
    Passenger p2(100, "Beth");
    Passenger p3(0, "Carl");

    repo.save(p1);
    repo.save(p2);
    repo.save(p3);

    EXPECT_EQ(101, p3.getId());
}

TEST_F(InMemoryPassengerRepositoryTest, SaveAfterDeleteMaintainsNextId) {
    Passenger p1(0, "Dana");
    Passenger p2(0, "Evan");

    repo.save(p1);
    repo.save(p2);

    repo.deletePassenger(p1.getId());

    Passenger p3(0, "Fiona");
    repo.save(p3);

    EXPECT_EQ(3, p3.getId());
}