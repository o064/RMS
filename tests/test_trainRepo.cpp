#include <gtest/gtest.h>
#include "Repo/InMemoryTrainRepository.h"
#include <memory>
#include <algorithm>
#include <stdexcept>

class InMemoryTrainRepositoryTest : public ::testing::Test {
protected:
    InMemoryTrainRepository repo;

    void SetUp() override {
        repo.clear();
    }

    void TearDown() override {
        repo.clear();
    }
};

// ===================== Basic Save Tests =====================

TEST_F(InMemoryTrainRepositoryTest, SaveAndGetTrainById) {
    Train t1(0, "Express", 20);
    repo.save(t1);

    EXPECT_EQ(t1.getTrainName(), "Express");
    EXPECT_EQ(t1.getTotalSeats(), 20);
    EXPECT_GT(t1.getTrainId(), 0);

    Train fetched = repo.getTrainById(t1.getTrainId());
    EXPECT_EQ(fetched.getTrainName(), "Express");
    EXPECT_EQ(fetched.getTrainId(), t1.getTrainId());
}

TEST_F(InMemoryTrainRepositoryTest, SaveModifiesOriginalTrainId) {
    Train t(0, "Test", 10);
    EXPECT_EQ(t.getTrainId(), 0);

    repo.save(t);

    // After save, the original train should have its ID updated
    EXPECT_GT(t.getTrainId(), 0);
}

// ===================== Auto-Increment ID Tests =====================

TEST_F(InMemoryTrainRepositoryTest, AutoIncrementId) {
    Train t1(0, "Express", 20);
    repo.save(t1);

    Train t2(0, "Regional", 15);
    repo.save(t2);

    EXPECT_EQ(t1.getTrainId(), 1);
    EXPECT_EQ(t2.getTrainId(), 2);
}

TEST_F(InMemoryTrainRepositoryTest, AutoIncrementSequential) {
    std::vector<int> ids;

    for (int i = 0; i < 10; ++i) {
        Train t(0, "Train" + std::to_string(i), 10);
        repo.save(t);
        ids.push_back(t.getTrainId());
    }

    // Verify sequential IDs: 1, 2, 3, ..., 10
    for (size_t i = 0; i < ids.size(); ++i) {
        EXPECT_EQ(ids[i], i + 1);
    }
}

TEST_F(InMemoryTrainRepositoryTest, AutoIncrementAfterDelete) {
    Train t1(0, "Train1", 10);
    repo.save(t1);
    EXPECT_EQ(t1.getTrainId(), 1);

    Train t2(0, "Train2", 10);
    repo.save(t2);
    EXPECT_EQ(t2.getTrainId(), 2);

    // Delete first train
    repo.deleteTrain(t1.getTrainId());

    // Next train should get ID 3, not reuse ID 1
    Train t3(0, "Train3", 10);
    repo.save(t3);
    EXPECT_EQ(t3.getTrainId(), 3);
}

// ===================== Explicit ID Tests =====================

TEST_F(InMemoryTrainRepositoryTest, SaveWithExplicitId) {
    Train t(42, "Explicit", 10);
    repo.save(t);

    EXPECT_EQ(t.getTrainId(), 42);
}

TEST_F(InMemoryTrainRepositoryTest, ExplicitIdDoesNotChangeNextId_IfLower) {
    // Save train with explicit ID lower than next_id
    Train t1(0, "Auto1", 10);
    repo.save(t1); // Gets ID 1, next_id becomes 2

    Train t2(0, "Auto2", 10);
    repo.save(t2); // Gets ID 2, next_id becomes 3

    // Now save with explicit ID 1 (lower than next_id which is 3)
    Train t3(1, "Explicit", 10);
    repo.save(t3); // Should update train with ID 1

    // Next auto-generated should still be 3
    Train t4(0, "Auto3", 10);
    repo.save(t4);
    EXPECT_EQ(t4.getTrainId(), 3);
}

TEST_F(InMemoryTrainRepositoryTest, ExplicitIdUpdatesNextId_IfHigher) {
    // Save train with ID 1
    Train t1(0, "Auto", 10);
    repo.save(t1);
    EXPECT_EQ(t1.getTrainId(), 1);

    // Save train with explicit high ID
    Train t2(100, "High", 10);
    repo.save(t2);
    EXPECT_EQ(t2.getTrainId(), 100);

    // Next auto-generated should be 101
    Train t3(0, "Auto2", 10);
    repo.save(t3);
    EXPECT_EQ(t3.getTrainId(), 101);
}

TEST_F(InMemoryTrainRepositoryTest, ExplicitIdEqualToNextId) {
    Train t1(0, "Auto", 10);
    repo.save(t1); // ID 1, next_id = 2

    // Save with explicit ID equal to next_id
    Train t2(2, "Explicit", 10);
    repo.save(t2);
    EXPECT_EQ(t2.getTrainId(), 2);

    // Next auto should be 3
    Train t3(0, "Auto2", 10);
    repo.save(t3);
    EXPECT_EQ(t3.getTrainId(), 3);
}

TEST_F(InMemoryTrainRepositoryTest, MultipleExplicitHighIds) {
    Train t1(50, "Train50", 10);
    repo.save(t1); // next_id becomes 51

    Train t2(100, "Train100", 10);
    repo.save(t2); // next_id becomes 101

    Train t3(75, "Train75", 10);
    repo.save(t3); // next_id stays 101 (75 < 101)

    Train t4(0, "Auto", 10);
    repo.save(t4);
    EXPECT_EQ(t4.getTrainId(), 101);
}

TEST_F(InMemoryTrainRepositoryTest, NegativeExplicitId) {
    Train t(-5, "Negative", 10);
    repo.save(t);

    EXPECT_EQ(t.getTrainId(), -5);

    // Next auto-increment should start from 1
    Train t2(0, "Auto", 10);
    repo.save(t2);
    EXPECT_EQ(t2.getTrainId(), 1);
}

TEST_F(InMemoryTrainRepositoryTest, VeryLargeExplicitId) {
    Train t(999999, "Large", 10);
    repo.save(t);

    EXPECT_EQ(t.getTrainId(), 999999);

    Train t2(0, "Auto", 10);
    repo.save(t2);
    EXPECT_EQ(t2.getTrainId(), 1000000);
}

// ===================== Update Tests =====================

TEST_F(InMemoryTrainRepositoryTest, UpdateExistingTrain) {
    Train t1(0, "Express", 20);
    repo.save(t1);
    int originalId = t1.getTrainId();

    // Create updated train with same ID
    Train updatedTrain(originalId, "Super Express", 30);
    repo.save(updatedTrain);

    EXPECT_EQ(updatedTrain.getTrainId(), originalId);
    EXPECT_EQ(updatedTrain.getTrainName(), "Super Express");
    EXPECT_EQ(updatedTrain.getTotalSeats(), 30);

    // Verify only one train exists
    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 1);
}

TEST_F(InMemoryTrainRepositoryTest, UpdatePreservesId) {
    Train t1(0, "Original", 10);
    repo.save(t1);
    int originalId = t1.getTrainId();

    // Modify and save again with same ID
    Train updated(originalId, "Updated", 20);
    repo.save(updated);

    EXPECT_EQ(updated.getTrainId(), originalId);
    EXPECT_EQ(updated.getTrainName(), "Updated");
    EXPECT_EQ(updated.getTotalSeats(), 20);
}

TEST_F(InMemoryTrainRepositoryTest, UpdateDoesNotChangeNextId) {
    Train t1(0, "Train1", 10);
    repo.save(t1); // ID 1, next_id = 2

    Train t2(0, "Train2", 10);
    repo.save(t2); // ID 2, next_id = 3

    // Update first train
    Train updated(t1.getTrainId(), "Updated", 20);
    repo.save(updated);

    // Next auto-generated should still be 3
    Train t3(0, "Train3", 10);
    repo.save(t3);
    EXPECT_EQ(t3.getTrainId(), 3);
}

TEST_F(InMemoryTrainRepositoryTest, UpdateWithSeatAllocatorState) {
    Train t(0, "Test", 5);
    auto allocator = t.getSeatAllocator();
    allocator->allocateSeat(101);
    allocator->allocateSeat(102);

    repo.save(t);
    int trainId = t.getTrainId();

    // Verify state
    EXPECT_EQ(t.getSeatAllocator()->getAvailableSeatCount(), 3);

    // Create updated train with more allocations
    Train fetched = repo.getTrainById(trainId);
    fetched.getSeatAllocator()->allocateSeat(103);

    repo.save(fetched);

    // Verify updated state
    Train updated = repo.getTrainById(trainId);
    EXPECT_EQ(updated.getSeatAllocator()->getAvailableSeatCount(), 2);
}

TEST_F(InMemoryTrainRepositoryTest, UpdateMultipleTimes) {
    Train t(0, "Original", 10);
    repo.save(t);
    int id = t.getTrainId();

    // Update 5 times
    for (int i = 1; i <= 5; ++i) {
        Train update(id, "Version" + std::to_string(i), 10 + i);
        repo.save(update);
    }

    // Verify final state
    Train final = repo.getTrainById(id);
    EXPECT_EQ(final.getTrainName(), "Version5");
    EXPECT_EQ(final.getTotalSeats(), 15);

    // Only one train should exist
    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 1);
}

// ===================== Delete Tests =====================

TEST_F(InMemoryTrainRepositoryTest, DeleteTrain) {
    Train t(0, "Express", 20);
    repo.save(t);
    int id = t.getTrainId();

    bool deleted = repo.deleteTrain(id);
    EXPECT_TRUE(deleted);

    EXPECT_THROW(repo.getTrainById(id), std::runtime_error);
}

TEST_F(InMemoryTrainRepositoryTest, DeleteNonExistentTrain) {
    bool result = repo.deleteTrain(999);
    EXPECT_FALSE(result);
}

TEST_F(InMemoryTrainRepositoryTest, DeleteAllTrains) {
    Train t1(0, "Train1", 10);
    Train t2(0, "Train2", 10);
    Train t3(0, "Train3", 10);

    repo.save(t1);
    repo.save(t2);
    repo.save(t3);

    EXPECT_TRUE(repo.deleteTrain(t1.getTrainId()));
    EXPECT_TRUE(repo.deleteTrain(t2.getTrainId()));
    EXPECT_TRUE(repo.deleteTrain(t3.getTrainId()));

    auto all = repo.getAllTrains();
    EXPECT_TRUE(all.empty());
}

TEST_F(InMemoryTrainRepositoryTest, DeleteSameTrainTwice) {
    Train t(0, "Test", 10);
    repo.save(t);
    int id = t.getTrainId();

    EXPECT_TRUE(repo.deleteTrain(id));
    EXPECT_FALSE(repo.deleteTrain(id)); // Second delete should return false
}

TEST_F(InMemoryTrainRepositoryTest, DeleteWithExplicitId) {
    Train t(42, "Explicit", 10);
    repo.save(t);

    EXPECT_TRUE(repo.deleteTrain(42));
    EXPECT_THROW(repo.getTrainById(42), std::runtime_error);
}

// ===================== GetTrainById Tests =====================

TEST_F(InMemoryTrainRepositoryTest, GetNonExistentTrain) {
    EXPECT_THROW(repo.getTrainById(999), std::runtime_error);
}

TEST_F(InMemoryTrainRepositoryTest, GetTrainByIdReturnsCorrectTrain) {
    Train t1(0, "Train1", 10);
    Train t2(0, "Train2", 20);
    Train t3(0, "Train3", 30);

    repo.save(t1);
    repo.save(t2);
    repo.save(t3);

    Train fetched2 = repo.getTrainById(t2.getTrainId());
    EXPECT_EQ(fetched2.getTrainName(), "Train2");
    EXPECT_EQ(fetched2.getTotalSeats(), 20);
}

TEST_F(InMemoryTrainRepositoryTest, GetTrainByIdReturnsCopy) {
    Train t(0, "Original", 10);
    repo.save(t);
    int id = t.getTrainId();

    Train fetched = repo.getTrainById(id);
    fetched.setTrainName("Modified");

    // Original in repo should be unchanged
    Train original = repo.getTrainById(id);
    EXPECT_EQ(original.getTrainName(), "Original");
}

TEST_F(InMemoryTrainRepositoryTest, GetTrainWithNegativeId) {
    Train t(-10, "Negative", 10);
    repo.save(t);

    Train fetched = repo.getTrainById(-10);
    EXPECT_EQ(fetched.getTrainId(), -10);
    EXPECT_EQ(fetched.getTrainName(), "Negative");
}

TEST_F(InMemoryTrainRepositoryTest, GetTrainByZeroId) {
    // Note: ID 0 should never exist as it's auto-incremented
    EXPECT_THROW(repo.getTrainById(0), std::runtime_error);
}

// ===================== GetAllTrains Tests =====================

TEST_F(InMemoryTrainRepositoryTest, GetAllTrains) {
    Train t1(0, "Express", 20);
    Train t2(0, "Regional", 15);

    repo.save(t1);
    repo.save(t2);

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);

    std::vector<std::string> names;
    for (const auto& train : all) {
        names.push_back(train.getTrainName());
    }

    EXPECT_NE(std::find(names.begin(), names.end(), "Express"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "Regional"), names.end());
}

TEST_F(InMemoryTrainRepositoryTest, GetAllTrainsEmpty) {
    auto all = repo.getAllTrains();
    EXPECT_TRUE(all.empty());
}

TEST_F(InMemoryTrainRepositoryTest, GetAllTrainsAfterDelete) {
    Train t1(0, "Train1", 10);
    Train t2(0, "Train2", 15);
    Train t3(0, "Train3", 20);

    repo.save(t1);
    repo.save(t2);
    repo.save(t3);

    repo.deleteTrain(t1.getTrainId());

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);
}

// ===================== Clear Tests =====================

TEST_F(InMemoryTrainRepositoryTest, ClearRepository) {
    Train t1(0, "Express", 20);
    Train t2(0, "Regional", 15);

    repo.save(t1);
    repo.save(t2);

    repo.clear();
    auto all = repo.getAllTrains();
    EXPECT_TRUE(all.empty());
}

TEST_F(InMemoryTrainRepositoryTest, ClearEmptyRepository) {
    repo.clear();
    auto all = repo.getAllTrains();
    EXPECT_TRUE(all.empty());
}

TEST_F(InMemoryTrainRepositoryTest, ClearResetsNextId) {
    Train t1(0, "Train1", 10);
    repo.save(t1);
    EXPECT_EQ(t1.getTrainId(), 1);

    repo.clear();

    // After clear, next_id should reset to 1
    Train t2(0, "Train2", 15);
    repo.save(t2);
    EXPECT_EQ(t2.getTrainId(), 1);

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 1);
}

// ===================== Seat Allocator Tests =====================

TEST_F(InMemoryTrainRepositoryTest, SeatAllocatorWorks) {
    Train t(0, "Express", 5);
    repo.save(t);

    EXPECT_TRUE(t.hasAvailableSeats());
    EXPECT_EQ(t.getSeatAllocator()->getAvailableSeatCount(), 5);

    auto allocator = t.getSeatAllocator();
    for (int i = 1; i <= 5; ++i) {
        int seatNumber = allocator->allocateSeat(100 + i);
        EXPECT_GT(seatNumber, 0);
    }

    EXPECT_FALSE(t.hasAvailableSeats());
    EXPECT_EQ(allocator->getAvailableSeatCount(), 0);
}

TEST_F(InMemoryTrainRepositoryTest, SeatAllocationAndFreeing) {
    Train t(0, "Test Train", 3);
    repo.save(t);
    int trainId = t.getTrainId();

    auto seatAllocator = t.getSeatAllocator();
    int seat1 = seatAllocator->allocateSeat(101);
    int seat2 = seatAllocator->allocateSeat(102);

    EXPECT_EQ(seatAllocator->getAvailableSeatCount(), 1);

    // Save back to repository
    repo.save(t);

    // Fetch and verify persistence
    Train fetched = repo.getTrainById(trainId);
    EXPECT_EQ(fetched.getSeatAllocator()->getAvailableSeatCount(), 1);
}

TEST_F(InMemoryTrainRepositoryTest, PreservesSeatAllocatorState) {
    Train t(0, "State Test", 3);
    auto allocator = t.getSeatAllocator();
    allocator->allocateSeat(101);
    allocator->allocateSeat(102);

    repo.save(t);

    EXPECT_EQ(t.getSeatAllocator()->getAvailableSeatCount(), 1);

    Train fetched = repo.getTrainById(t.getTrainId());
    EXPECT_EQ(fetched.getSeatAllocator()->getAvailableSeatCount(), 1);
}

// ===================== Edge Cases and Boundary Tests =====================

TEST_F(InMemoryTrainRepositoryTest, DuplicateTrainNames) {
    Train t1(0, "Express", 10);
    Train t2(0, "Express", 15);

    repo.save(t1);
    repo.save(t2);

    EXPECT_NE(t1.getTrainId(), t2.getTrainId());

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);
}

TEST_F(InMemoryTrainRepositoryTest, SaveAndUpdateAlternating) {
    Train t1(0, "Train1", 10);
    repo.save(t1);

    Train t2(0, "Train2", 20);
    repo.save(t2);

    // Update first
    Train update1(t1.getTrainId(), "Updated1", 15);
    repo.save(update1);

    // Update second
    Train update2(t2.getTrainId(), "Updated2", 25);
    repo.save(update2);

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);
}

TEST_F(InMemoryTrainRepositoryTest, MultipleOperations) {
    Train t1(0, "Train A", 10);
    Train t2(0, "Train B", 15);
    Train t3(0, "Train C", 20);

    repo.save(t1);
    repo.save(t2);
    repo.save(t3);

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 3);

    bool deleted = repo.deleteTrain(t2.getTrainId());
    EXPECT_TRUE(deleted);

    all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 2);

    std::vector<std::string> names;
    for (const auto& train : all) {
        names.push_back(train.getTrainName());
    }
    EXPECT_NE(std::find(names.begin(), names.end(), "Train A"), names.end());
    EXPECT_NE(std::find(names.begin(), names.end(), "Train C"), names.end());
    EXPECT_EQ(std::find(names.begin(), names.end(), "Train B"), names.end());
}

TEST_F(InMemoryTrainRepositoryTest, SaveManyTrains) {
    const int COUNT = 100;
    std::vector<int> ids;

    for (int i = 0; i < COUNT; ++i) {
        Train t(0, "Train" + std::to_string(i), i + 1);
        repo.save(t);
        ids.push_back(t.getTrainId());
    }

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), COUNT);

    // All IDs should be sequential
    for (int i = 0; i < COUNT; ++i) {
        EXPECT_EQ(ids[i], i + 1);
    }
}

TEST_F(InMemoryTrainRepositoryTest, MixedExplicitAndAutoIds) {
    Train t1(0, "Auto1", 10);
    repo.save(t1); // ID 1
    EXPECT_EQ(t1.getTrainId(), 1);

    Train t2(50, "Explicit50", 10);
    repo.save(t2); // ID 50, next_id = 51

    Train t3(0, "Auto2", 10);
    repo.save(t3); // ID 51
    EXPECT_EQ(t3.getTrainId(), 51);

    Train t4(25, "Explicit25", 10);
    repo.save(t4); // ID 25, next_id stays 52

    Train t5(0, "Auto3", 10);
    repo.save(t5); // ID 52
    EXPECT_EQ(t5.getTrainId(), 52);

    auto all = repo.getAllTrains();
    EXPECT_EQ(all.size(), 5);
}

TEST_F(InMemoryTrainRepositoryTest, SaveWithZeroSeats) {
    Train t(0, "ZeroSeats", 0);
    repo.save(t);

    EXPECT_EQ(t.getTotalSeats(), 0);
    // Allocator should default to 10
    EXPECT_EQ(t.getSeatAllocator()->getAvailableSeatCount(), 10);
}

TEST_F(InMemoryTrainRepositoryTest, SaveWithNegativeSeats) {
    Train t(0, "NegativeSeats", -5);
    repo.save(t);

    EXPECT_EQ(t.getTotalSeats(), -5);
    // Allocator should default to 10
    EXPECT_EQ(t.getSeatAllocator()->getAvailableSeatCount(), 10);
}

TEST_F(InMemoryTrainRepositoryTest, SaveEmptyNameTrain) {
    Train t(0, "", 10);
    repo.save(t);

    EXPECT_EQ(t.getTrainName(), "");
    Train fetched = repo.getTrainById(t.getTrainId());
    EXPECT_EQ(fetched.getTrainName(), "");
}