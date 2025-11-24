//
// Created by Omar on 11/24/2025.
//
#include <gtest/gtest.h>
#include "Repo/InMemoryTicketRepository .h"
#include "models/Ticket.h"
#include "models/Passenger.h"

class TicketRepositoryTest : public ::testing::Test {
protected:
    InMemoryTicketRepository repo;
    Passenger testPassenger;

    TicketRepositoryTest() : testPassenger(1, "John Doe") {}

    void SetUp() override {
        repo.clear();
    }

    void TearDown() override {
        repo.clear();
    }
};

TEST_F(TicketRepositoryTest, SaveTicket_GeneratesId) {
    Ticket t(0, 1, 101, testPassenger);
    repo.save(t);

    EXPECT_GT(t.getId(), 0);
    EXPECT_EQ(t.getSeat(), 1);
}

TEST_F(TicketRepositoryTest, GetTicketById_Success) {
    Ticket t(0, 5, 200, testPassenger);
    repo.save(t);

    auto retrieved = repo.getTicketById(t.getId());

    // Based on your repo code, getTicketById returns optional or throws
    // Your implementation throws runtime_error if not found, but returns optional if found
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(retrieved->getId(), t.getId());
    EXPECT_EQ(retrieved->getPassenger().getName(), "John Doe");
}

TEST_F(TicketRepositoryTest, GetTicketById_ThrowsWhenNotFound) {
    // Your implementation explicitly throws runtime_error for ID lookup failures
    EXPECT_THROW(repo.getTicketById(999), std::runtime_error);
}

TEST_F(TicketRepositoryTest, GetTicketByTrainAndPassenger_Success) {
    int trainId = 100;
    Passenger p(50, "Jane");
    Ticket t(0, 10, trainId, p);
    repo.save(t);

    auto result = repo.getTicketByTrainAndPassenger(trainId, p.getId());

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->getId(), t.getId());
}

TEST_F(TicketRepositoryTest, GetTicketByTrainAndPassenger_ReturnsNulloptWhenNotFound) {
    // Your implementation returns std::nullopt here (doesn't throw)
    auto result = repo.getTicketByTrainAndPassenger(100, 999);
    EXPECT_FALSE(result.has_value());
}

TEST_F(TicketRepositoryTest, DeleteTicket_Success) {
    Ticket t(0, 1, 100, testPassenger);
    repo.save(t);
    int id = t.getId();

    bool deleted = repo.deleteTicket(id);
    EXPECT_TRUE(deleted);

    EXPECT_THROW(repo.getTicketById(id), std::runtime_error);
}

TEST_F(TicketRepositoryTest, GetAllTickets) {
    Ticket t1(0, 1, 100, testPassenger);
    Ticket t2(0, 2, 100, testPassenger);
    repo.save(t1);
    repo.save(t2);

    auto all = repo.getAllTickets();
    EXPECT_EQ(all.size(), 2);
}